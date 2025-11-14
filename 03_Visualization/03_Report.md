# Visualizing C++ Programs Output Efficiently
### *A Minimal Pipeline for Clean Financial Visualizations*

© 2025 Yvan Richard

---

## Table of Contents

1. [Introduction](#1-introduction)



## 1. Introduction

In this section of the project, I use the libraries I built and published in [01_Exact_Pricing_Methods](/01_Exact_Pricing_Methods/) to produce standard financial visualizations. Unlike the Baruch Certificate workflow, which relies on Windows-only Excel automation through COM interfaces, I adopt a clean, cross-platform pipeline where the C++ engine outputs structured CSV data and Python handles the visualization. In my humble opinion, this design is objectively superior since it removes brittle system dependencies, avoids the overhead of automating Excel, and allows for faster iteration and reproducible results. More importantly, it separates computation from presentation, enabling precise control over plotting.
Furthermore, every one will be able to only retrieve the CSV data sets if they really want to use Excel.

To demonstrate the procedure to follow, I will plot two surfaces: a price surface and a delta surface for a European call option.

## 2. Structure & Procedure

This sub project is structured as follow:

```bash
03_Visualization/
├── 03_Report.md
├── cpp_generation/
│   └── main.cpp
├── py_visualization
│   └── plots_generation.py
├── data/
│   ├── price_surface.csv
│   └── delta_surface.csv
├── images/
│   ├── option_price_surface.png
│   └── delta_surface.png

```
In the [cpp_generation](/03_Visualization/cpp_generation/) file, I coded two functions to write to CSV files with an `fstream` function the results of my
pricing engine. The program [main.cpp](/03_Visualization/cpp_generation/main.cpp) should be run with the configuration that can be found in the .vscode subfolder also located in [cpp_generation](/03_Visualization/cpp_generation/). In the program, the user is prompted to register base values (config I used for my plots):

```bash
Option Price and Delta Surface Generator
-----------------------------------------
(The spot price and the time to maturity will vary)
Enter the strike price K: 110
Enter the time to maturity: 0.40
Enter the interest rate: 0.05
Enter the cost of carry: 0.05
Enter the asset price: 100   
Enter the volatility: 0.35
```
and then I changed the spot price and the time to maturity around the entered values ($\pm 0.5 \cdot x$) for having a range. Once this is done, the `data` subfolder is automatically filled with two CSV data sets. Then, we go into the [py_visualization](/03_Visualization/py_visualization/) subfolder:

```bash
cd py_visualization
```

we activate the virtual environment I created with the relevant dependencies:

```bash
source .venv/bin/activate
```

and we run the [code](/03_Visualization/py_visualization/plots_generation.py):

```bash
python3 plots_generation.py
```
which will automatically generate the surfaces we wanted in the subfolder [images](/03_Visualization/images/). For more detail on the actual implementation, feel free to consult the program scripts.

## 3. Results

Finally, we are able to integrate our results dynamically here in this markdown script. The price surface of the European call option:

![](/03_Visualization/images/option_price_surface.png)

And the delta surface of this same option:

![](/03_Visualization/images/delta_surface.png)

# The End

**Produced and written by Yvan Richard**
