# Monte Carlo Pricing Methods
### *Discussion on Methods and Applications Based on Daniel Duffy Code Base*

© 2025 Yvan Richard

---

## Table of Contents

1. [Introduction](#1-introduction)
2. [Monte Carlo 101](#2-monte-carlo-101)  
    2.1. [General Definition](#21-general-definition)  
    2.2. [The Computational Finance Approach](#22-the-computational-finance-approach)   
    2.3. [Code Implementation](#23-code-implementation)  
    2.4. [Hyperparameters of the Monte Carlo Algorithm](#24-hyperparameters-of-the-monte-carlo-algorithm)
3. [Advanced Monte Carlo](#3-advanced-monte-carlo)  
    3.1. [Goals](#31-goals)   
    3.2. [Code Implementations](#32-code-implementations--se)   
    3.3. [Influence of the Hyperparameters](#33-influence-of-the-hyperparameters-on--and-se)


    


## 1. Introduction

In this section of the project, I did not build myself a Monte Carlo engine to price options but I discuss and address some questions based on the code of Daniel Duffy,
the engineer in charge of the code base for the *C++ Programming for Financial Engineering Certificate*. In later
works, I will implement my own version of this engine.

The first part of our report will be focused on the basic implementation of Monte Carlo methods for derivatives pricing
and we focus on more advanced topics in the second section.

## 2. Monte Carlo 101

### 2.1. General Definition

Here is a very brief and general description of what are Monte Carlo methods: 

> **Monte Carlo methods**, (sometimes called Monte Carlo experiments or Monte Carlo simulations) are a broad class of computational algorithms that rely on repeated random sampling to obtain numerical results. The underlying concept is to use randomness to solve problems that might be deterministic in principle. The name comes from the Monte Carlo Casino in Monaco, where the primary developer of the method, mathematician Stanisław Ulam, was inspired by his uncle's gambling habits.

[Wikipedia. (n.d.). *Monte Carlo Method*. Retrieved on 11.11.2025 at 19:37 UTC.](https://en.wikipedia.org/wiki/Monte_Carlo_method)

### 2.2. The Computational Finance Approach

**Overview**

In our case, we examine the case of a
one-factor European call option using the assumptions of the original Black Scholes equation. I will explain our approach closely based on QuantNet (2025).

For a European option at time $t = T$, where $T$ is the maturity date, or the exercise date, we know the value of the option as a function of
its strike price $K$ and the current stock price $S_T$.

Hence, to find the value of our (call) option at time $t = 0$: $C_0$, we will estimate the stock price at time $T$ with Monte Carlo methods,
infer the price of the call option $C_T$ and discount back in time to find $C_0$. Basically, the stock price $S_T$ will be estimated $m$ times (this is the essence of the Monte Carlo procedure, repeated random sampling) and then we will aggregate these $m$ predictions into one consolidated estimate:

$$
\hat{S}_T = \frac{1}{m} \sum_{i = 1}^{m} S_{T, i}^{MC}
$$

where $S_{T, i}^{MC}$ is the $i$-th esimates with Monte Carlo method. Finally, we discount back $\hat{S}_T$ to find $\hat{S}_0$.

**Discrete Time**

The evolution of a stock price is a time-dependent continuous process. However, in our method we discretize the time intervall $[0, T]$ into
equal length intervals $\Delta t$. Once this has been established, we consider the scalar linear stochastic differential equation (SDE):

$$
\begin{equation}
dX = aXdt + bXdW, \quad a, b \in \mathbb{R}, \quad X(0) = A
\end{equation}
$$

We write equation $(1)$ as a stochastic integral equation between the (arbitrary) times $s$ and $t$ as follows:

$$
\begin{equation}
X(t) = X(s) + \int_{s}^{t} aX(y)dy + \int_{s}^{t} bX(y)dW(y), \quad s < t
\end{equation}
$$

where the first term, $\int_s^t aX_ydy$, is a standard Riemann integral; it accumulates the deterministic drift component and 
the second term, $\int_s^t bX_y\,dW_y$, is a stochastic (Itô) integral, it accumulates the random shocks driven by the Wiener process. Essentially, the integral form describes how to compute $X(t)$ over a finite interval by “summing” infinitesimal changes.

Then, we evalute this equation at two different discrete points $t_n$
and $t_{n+1}$ and we obtain the following identity:

$$
\begin{equation}
X(t_{n+1}) = X(t_n) + a\int_{t_n}^{t_{n+1}} X(y)dy \;+\; b\int_{t_n}^{t_{n+1}} X(y)dW(y)
\end{equation}
$$

We now approximate $(3)$ with its discrete counterpart $Y(t)$:

$$
\begin{equation}
Y(n+1) = Y(n) + aY_n\Delta t_n \;+\; b Y_n \Delta W_n
\end{equation}
$$

where:

$$
\Delta W_n = W(t_{n+1}) - W(t_n)
$$

this approximation is generally known as the [Euler–Maruyama method](https://en.wikipedia.org/wiki/Euler–Maruyama_method). Furthermore, in
many applications, the increments are computed as:

$$
\begin{equation}
\begin{cases}
\Delta t_n &= \Delta t = T/N \quad \text{(constant time steps)} \\
\Delta W_n &= \sqrt{\Delta t} \cdot z_t, \quad z_t \sim \mathcal{N}(0, 1)
\end{cases}
\end{equation}
$$

Here, we can rely on the `Boost` libraries to generate the increments $\Delta W_i$.

### 2.3. Code Implementation

*(Disclaimer: All the code in this subsection is adapted from Daniel Duffy’s teaching code; explanations are mine.)*

**OptionData**

First, we are provided with a lighweight struct called `OptionData` that stores the parameters of the European option and also implement a member function `myPayOffFunction()` to compute the payoff of the according to its type (call, put). The most important numerical parameters stored are :

```cpp
double K, T, r, sig;
```

where $K$ is the strike price, $T$ is the exercise date, $r$ the risk-free interest rate, and $\sigma$ the volatility of the underlying.

**Stochastic Differential Equation**

Subsequently, Duffy defines within the namespace `X` some useful implementations for the characteristic equation governing the dynamics of the asset price, specifically in the Black-Scholes (or CEV) model.

Indeed, in this model, the SDE is given by what we studied (here with
a familiar notation for drift and diffusion):

$$
\begin{equation}
dS_t = r S_t\, dt + \sigma S_t^{\beta_{CEV}} dW_t
\end{equation}
$$

in our precedent explanations, we had $S_t = X_t, a= r, \sigma = b$, and $\beta_{CEV} = 1$. The *deterministic* part of this SDE is the drift:

$$
\text{drift} \;=\; r S_t\, dt
$$

and was coded by Duffy as:

```cpp
double drift(double t, double X)
{
    return (data->r)*X; // r - D
}
```
where `data` is a pointer to an `OptionData` struct. The parameter `double t` is not used for the moment.

Then, we have the diffusion $\sigma S_t^{\beta_{CEV}} dW_t$ coded as:

```cpp
double diffusion(double t, double X)
{
    double betaCEV = 1.0; // classic BS model
    return data->sig * pow(X, betaCEV);
}
```

and its respective derivative (needed later):

```cpp
double diffusionDerivative(double t, double X)
{
    double betaCEV = 1.0;
    return 0.5 * (data->sig) * (betaCEV) * pow(X, 2.0 * betaCEV - 1.0);
}
```

Once all of this is in place, Duffy implements a basic Monte Carlo algorithm where he computes `NSim` price paths. Furthermore, each price path is composed of `N` $+ 1$ data points, meaning that the time intervals are of size:
```cpp
double k = myOption.T / double (N);
```

Then each individual price path is computed in a vector `x` as:

```cpp
for (unsigned long index = 1; index < x.size(); ++index)
{
    // Create a random number
    dW = myNormal->getNormal();
        
    // The FDM (in this case explicit Euler)
    VNew = VOld  + (k * drift(x[index-1], VOld))
                + (sqrk * diffusion(x[index-1], VOld) * dW);

    // Update to new value
    VOld = VNew;

    // Spurious values
    if (VNew <= 0.0) coun++;
}
```

The key component here is:
```cpp
VNew = VOld  + (k * drift(x[index-1], VOld))
                + (sqrk * diffusion(x[index-1], VOld) * dW);
```

We understand that storing the path into a vector is a convenient approach
since we can easily access $Y_{n}$ to compute $Y_{n + 1}$. Basically, this line excatly implements our equation $(4)$ with the configuration given by
equation $(5)$. We note that Duffy uses the boost libraries to draw a random
number from the standard normal distribution `dW`. Once the final price $S_{T, i}$
on the price path $i$ has been computed, Duffy computes the payoff and adds it to the `price` variable that will, at the end of the `NSim` simulations
holds the approximation for the option price:

```cpp
for (long i = 1; i <= NSim; ++i)
{
    // Always start price path at S_0
    VOld = S_0;
    for (unsigned long index = 1; index < x.size(); ++index)
    {
        // [compute price path as shown above]
    }
    
    // compute payoff
    double tmp = myOption.myPayOffFunction(VNew);

    // compute the average of the predictions on the go
    price += (tmp)/double(NSim);
}
```

Finally, once that `price` has been computed after the `NSim` iterations of the outter for loop, we discount back to compute the price of the option at $t = 0$:

$$
C_0 = \underbrace{\left( \frac{1}{m} \sum_{i = 1}^{m} C_{i, T} \right)}_{price} \cdot e^{-rT}
$$

Equivalently in code:

```cpp
price *= exp(-myOption.r * myOption.T);
```

The results are then ready to be printed. Here is the full console print of a typical use of the entire program:

```txt
1 factor MC with explicit Euler
Number of subintervals in time: 500 
Number of simulations: 10000
10000
Price, after discounting: 5.93754, 
Number of times origin is hit: 0
```
*Note: last line prints to the console how many times during the Monte Carlo simulation the simulated asset price $S_t$ (stored in VNew) hit or went below zero.*

### 2.4. Hyperparameters of the Monte Carlo Algorithm

**Hypothesis Formulation**

As we have seen above, our version of the Monte Carlo algorithm has two *hyperparameters* that we can tune based on our preferences. These two hyperparameters are: 

- `NSim` : $m$ : The number of simulation (simulated price paths) we want to compute.
- `N`: $n$ : number of time steps, i.e. roughly the number of data points that we have on each price path.

What we can already observe is that as $n \to \infty$, our discrete process converge to the SDE solution. Indeed, we recall that our current discrete version is supposed to be an approximation of the continuous phenomenon. Our time steps are computed on the interval $[0, T]$ as:

$$
t_k=\frac{kT}{n},\quad k=0,\dots,n,\qquad \Delta t_k:=t_{k+1}-t_k=\frac{T}{n}.
$$

So we can think that as $n \to \infty$, our end estimate $\hat C_0$ would be more precise. Now, if we pivot and observe the influence of `NSim`, $m$, on our estimates, we could also reasonably formulate the hypothesis that as 
$m \to \infty$, we have *seen* more possible price paths and the distribution of this price paths is reflected more accurately since we have seen many of them, thus the variance and accuracy of our estimates would be lower.

In substance, I posit that increasing by increasing $n$ and $m$ we can improve the performance of the model but it is not clear which one will contribute the most.

**Hypothesis Test**

Here, we do not develop a formal test for our hypothesis but will try it empirically with batch 1 and 2 and different values of $m$ and $n$. For $m$
and $n$, we will select three values for each and do a mesh grid with them.

$$
m \in \{50, 500, 5000, 50000\}
$$

$$
n \in \{50, 500, 5000, 50000\}
$$

and so our grid with pairs $(m, n)$ will be:

$$
\begin{bmatrix}
(50,50) & (50,500) & (50,5000) & (50,50000) \\
(500,50) & (500,500) & (500,5000) & (500,50000) \\
(5000,50) & (5000,500) & (5000,5000) & (5000,50000) \\
(50000,50) & (50000,500) & (50000,5000) & (50000,50000)
\end{bmatrix}
$$

and the data of batch 1 and 2 are given by:

```cpp
// batch 01 with C = 2.13337, P = 5.84628
double T = 0.25, K = 65, sig = 0.30, r = 0.08, S = 60;

// batch 02 with C = 7.96557, P = 7.96557
double T = 1.0, K = 100, sig = 0.2, r = 0.0, S = 100; 
```
we will mostly focuse on computing the price of the call option $C$ and we will observe how the relative error $\eta$ evolves:

$$
\eta(\hat C; C^\star)
=\frac{\lvert \hat C - C^\star\rvert}{\lvert C^\star\rvert},
$$

My results are reported here in two matrices (based on the same grid as above). The matrix $\mathbf{\hat C}$ are the results I obtained and the matrix $\mathbf{\Eta}$ compiles the different $\eta_i$.

$$
\mathbf{\hat C}_1 = 
\begin{bmatrix}
2.37058 & 1.76568 & 2.00841 & 1.25603 \\
1.87755 & 2.22587 & 1.73592 & 2.14897 \\
2.08218 & 2.01729 & 2.2048 & 2.05083 \\
2.07387 & 2.16105 & 2.1221 & 2.15596
\end{bmatrix}
$$

Then:

$$
\mathbf{\Eta}_1=
\begin{bmatrix}
0.11119 & 0.17235 & 0.05857 & 0.41125 \\
0.11991 & 0.04336 & 0.18630 & 0.00731 \\
0.02399 & 0.05441 & 0.03348 & 0.03869 \\
0.02789 & 0.01297 & 0.00528 & 0.01059
\end{bmatrix}
$$

We can produce the same matrix $\mathbf{\Eta}_2$ with batch 2:

$$
\mathbf{\hat C}_2 = 
\begin{bmatrix}
8.26902 & 6.71189 & 8.15105 & 4.65879 \\
7.43971 & 8.21827 & 6.75605 & 8.11088 \\
7.87125 & 7.59632 & 8.16036 & 7.69629 \\
7.81025 & 8.04316 & 7.92605 & 8.03097
\end{bmatrix}
$$

$$
\mathbf{\Eta}_2 =
\begin{bmatrix}
0.03810 & 0.15739 & 0.02329 & 0.41513 \\
0.06602 & 0.03172 & 0.15184 & 0.01824 \\
0.01184 & 0.04636 & 0.02445 & 0.03381 \\
0.01950 & 0.00974 & 0.00496 & 0.00821
\end{bmatrix}
$$

Of course, this method is not statistically solid and should only be viewed as a toy exploration of how the Monte Carlo algorithm behave when faced with changing $m$ and $n$. To have a better grasp of what is going on, I aggregated by taking the arithemtic mean of each grid point and built the matrix $\Eta$, where $\eta_{(1, 1)}$ is given by:

$$
\eta_{(1, 1)} = \frac{\eta_{n = 50, m = 50}^{b1} + \eta_{n = 50, m = 50}^{b2}}{2}
$$

where $bi$ indicates the batch. Then, I plotted the following surface based on my mesh grid and results:

![The Relative Error Surface](/02_Monte_Carlo/images/relative_error_surface.png)

and apparently, on our two previous batches, increasing $m$ is what appears to be most significant drive of a better accuracy.

**Finding an Estimate Close to Theoretical Value**

With these insights in mind, we will try to find the *lowest* pair $(m, n)$ which gets us closer to the theoretical value. As we have seen, it is computationally expensive to increase both $m$ and $n$ at the same rate. However, this time we will continue to increase $n$ but we will put our focus on increasing $m$ first.

As my numerous attempts revealed it, it is really hard to get a precision below the $10^{-2}$ threshold. With $(100, 1000000)$ I was able to get it but further increasing $m$ and $n$ yield not better result and start to be quite expensive to run. Furthermore, we might also face some floating point
precision issues.

**Stress Testing on Batch 04**

Now, we shift to new batch data, as given by batch 4:

```cpp
// batch 04 with C = 92.17570, P = 1.24750
double T = 30.0, K = 100.0, sig = 0.30, r = 0.08, S = 100.0;
```

which has the particularity of having a high $T$ in comparison to
other data sets. Obviously, our MC algoirthm will still behave in the same way as above when we change $m$ and $n$ but this time it might be harder to find good estimates. For instance, with the pair $(m, n) = (10^6, 10^2)$ we obtained for $\hat C$:

$$
\eta_{(10^6, 10^2)}^{b4} = \frac{|89.5241 - 92.17570|}{92.17570} \approx 0.0288
$$

and the same $\eta$ on batch 02 was around $0.00057$. My main idea for explaining this is that a larger maturity $T$ hurts accuracy because both bias and variance grow. With a fixed number of time steps $n$, the step size $\Delta t=T/n$ increases, so Euler–Maruyama accumulates more discretization error over a longer horizon. To find equivalently good results, we would probably need a higher (more computationally expensive) $n$. With the pair $(10^6, 10^3)$ I found an $\eta$ of $0.00663$.

## 3. Advanced Monte Carlo

### 3.1. Goals

In this section, we will rely on the same code base as above but this time I will code some methods to track sample standard deviation $\hat \sigma$ and standard error $SE(\hat \sigma)$ of our estimates. These metrics are computed as:

$$
\begin{equation}
\hat \sigma(C_0) = \sqrt{\frac{1}{m - 1}\sum_{i = 1}^{m} (C_{T, i} - \overline{C_{T}})^2} \cdot e^{-rT}
\end{equation}
$$

where:

$$
\overline{C_{T}} = \frac{1}{m} \sum_{i = 1}^{m} C_{T, i}
$$

and then the standard error is based on $\hat \sigma$ and computed as:

$$
\begin{equation}
SE(C_0) = \frac{\hat \sigma(C_0)}{\sqrt{m}}
\end{equation}
$$

### 3.2. Code Implementations: $\hat \sigma$, SE

If we desire to implement a generic version of the sample standard deviation for our estimates, a classic approach is:

```cpp
template <typename Type>
double sample_std(const std::vector<Type>& data, double r, double T)
{
	// mean
	double mean = std::accumulate(data.begin(), data.end(), 0.0) / data.size();

	// sum of squared differences
	double sq_sum = 0.0;
	for (const auto& value : data) {
		sq_sum += (value - mean) * (value - mean);
	}

	// final result
	return std::sqrt(sq_sum / (data.size() - 1)) * std::exp(-r * T);
}
```

Once the discounted sample standard deviation has been computed, it is straightforward to compute the standard error:

```cpp
template <typename Type>
double standard_error(const std::vector<Type>& data, double r, double T)
{
	double stddev = sample_std(data, r, T);
	return stddev / std::sqrt(data.size());
}
```

### 3.3. Influence of the Hyperparameters on $\hat \sigma$ and SE

Now, to study the influence of the hyperparameters I basically adopted the same approach has above but
I optimized the code to make my approach slightly more idiomatic and also be able to store the results of
my experiment in a CSV data set. My main goal was to observe how the above statistics behave with
changing values of $m$ and $n$. To make this more interactive, I visualized all my results in heatmaps by relying on Python. The code
of these visualizations can be found [here](/02_Monte_Carlo/notebooks/MC_stats_evolution.ipynb).

**Heatmaps**

Heatmaps of the discounted sample standard deviation in function of
$m$ and $n$:

![](/02_Monte_Carlo/images/heatmap_sd_batches.png)

Heatmaps of the standard error in function of
$m$ and $n$:

![](/02_Monte_Carlo/images/heatmap_se_batches.png)

Heatmaps of the relative error in function of $m$ and $n$:

![](/02_Monte_Carlo/images/heatmap_relerr_batches.png)

**Discussion of the Results**

From the runs in Batches 1–2, the sample standard deviation (SD) is essentially insensitive to the number of simulations $m$ and only weakly affected by the number of time steps $n$. By contrast, the standard error (SE) scales almost perfectly (according to the formula): holding $n$ fixed, increasing $m$ by $10 \times$ shrinks SE by $\approx \sqrt{10}$.

We see again that accuracy versus the exact price is driven primarily by $m$ and only secondarily by $n$ (discretization bias), with no strong monotone pattern in $n$ for these vanilla cases. With large $m$, relative errors become very small even at modest $n$. In Batch 2, $n=50 , m=500k$ yields $\eta = 0.00133$; raising $n$ to $5000$ at the same $m$ gives $0.00311$ (slightly worse), indicating time-discretization bias is already negligible at $n=50$. All in all, I think that the practical takeaway we should take is: tune $m$ first until SE is below our selected tolerance; then we adjust $n$ to remove residual time-discretization bias.

*Disclaimer: For this part, I will only publish the code I mostly implemented myself in [TestMC_std.cpp](). Since it relies on external
dependencies it will be impossible to run it directly but it still gives a good overview of what is going on under the hood.*


## References

QuantNet. (2025). *Level 9: Introductory Computational Finance*. C++ Programming for Financial Engineering Certificate.

[Wikipedia. (n.d.). *Monte Carlo Method*. Retrieved on 11.11.2025 at 19:37 UTC.](https://en.wikipedia.org/wiki/Monte_Carlo_method)

[Wikipedia. (n.d.). *Euler-Maruyama Method*. Retrieved on 11.11.2025 at 20:15 UTC.](https://en.wikipedia.org/wiki/Euler–Maruyama_method)


# The End

**Written by Yvan Richard**



