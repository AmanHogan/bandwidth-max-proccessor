import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors

def load_csv(file_path):
    """Load a CSV file into a DataFrame."""
    return pd.read_csv(file_path)

def plot_line_graph(df, x_label, y_label, title, output_file):
    """Plot a line graph with multiple series."""
    thread_columns = df.columns[1:]
    plt.figure(figsize=(10, 6))
    
    for _, row in df.iterrows():
        plt.plot(thread_columns, row[1:], marker='o', label=row[0])
    
    plt.title(title)
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.legend(title=df.columns[0])
    plt.grid(True)
    plt.savefig(output_file)


def plot_heatmap(df, x_label, y_label, title, output_file, cmap="coolwarm", color_bounds=None):
    """Plot a heatmap with the given DataFrame."""
    row_labels = df.iloc[:, 0].values
    col_labels = df.columns[1:]
    data = df.drop(df.columns[0], axis=1).to_numpy()
    heatmap_data = pd.DataFrame(data, index=row_labels, columns=col_labels)
    
    plt.figure(figsize=(12, 8))
    sns.heatmap(heatmap_data, annot=True, fmt=".2f", cmap=sns.color_palette("coolwarm", as_cmap=True), cbar_kws={'label': y_label})
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.title(title)
    plt.savefig(output_file)


def main():
    
    # Load data
    write_results = load_csv("../output/write_results.csv")
    read_results = load_csv("../output/read_results.csv")
    
    # Plot line graph for write results
    plot_line_graph(
        df=write_results,
        x_label='Number of Threads',
        y_label='Bandwidth (MB/s)',
        title='Bandwidth vs Number of Threads',
        output_file='../output/write_line_graph.png'
    )

    # Plot heatmap for read results
    plot_heatmap(
        df=read_results,
        x_label='Number of Threads',
        y_label='Unroll Loop Size',
        title='Heatmap of Bandwidth vs Unroll Loop Size and Threads',
        output_file='../output/read_heatmap.png'
    )

    # Plot heatmap for write results with a custom color map
    plot_heatmap(
        df=write_results,
        x_label='Number of Threads',
        y_label='Optimization Type',
        title='Heatmap of Write Bandwidth vs Optimization and Threads',
        output_file='../output/write_heatmap.png',
        cmap=sns.color_palette("coolwarm", as_cmap=True)
    )

if __name__ == "__main__":
    main()