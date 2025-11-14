# plots_generation.py

import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import pandas as pd
from mpl_toolkits.mplot3d import Axes3D

def plot_surface(df, title, xlabel, ylabel, zlabel):

    """
    This function generates a 3D surface plot from a dataframe.
    The dataframe should contain three columns corresponding to the x, y, and z axes.

    Furthemore, the df should be structured such that:
    - first column for x values (xlabel)
    - second column for y values (ylabel)
    - third column for z values (zlabel)
    """

    # pivot the dataframe to create a grid for surface plotting
    pivot_table = df.pivot(index=df.columns[1], columns=df.columns[0], values=df.columns[2])

    X = pivot_table.columns.values
    Y = pivot_table.index.values
    X, Y = np.meshgrid(X, Y)
    Z = pivot_table.values

    # plot the surface
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.plot_surface(X, Y, Z, cmap='viridis')
    ax.set_title(title)
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.set_zlabel(zlabel)

    # set the view angle for better visualization
    ax.view_init(elev=30, azim=120)

    plt.savefig(f"../images/{title.replace(' ', '_').lower()}.png", dpi = 500)
    plt.show()

def load_data(file_path):
    return pd.read_csv(file_path)

def main():

    # load the two datasets
    df1 = load_data('../data/price_surface.csv')
    df2 = load_data('../data/delta_surface.csv')

    # plot the surfaces
    plot_surface(df1, 'Option Price Surface', 'Asset Price', 'Time to Maturity', 'Call Option Price')
    plot_surface(df2, 'Delta Surface', 'Asset Price', 'Time to Maturity', 'Call Option Delta')

if __name__ == "__main__":
    main()

