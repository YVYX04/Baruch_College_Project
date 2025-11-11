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

Aa.

















## References

QuantNet. (2025). *Level 9: Introductory Computational Finance*. C++ Programming for Financial Engineering Certificate.

[Wikipedia. (n.d.). *Monte Carlo Method*. Retrieved on 11.11.2025 at 19:37 UTC.](https://en.wikipedia.org/wiki/Monte_Carlo_method)

[Wikipedia. (n.d.). *Euler-Maruyama Method*. Retrieved on 11.11.2025 at 20:15 UTC.](https://en.wikipedia.org/wiki/Euler–Maruyama_method)



