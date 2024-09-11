import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors

def load_csv(file_path):
    """Load a CSV file into a DataFrame."""
    return pd.read_csv(file_path, header=None)

def assign_thread_counts(df, rows_per_thread=5):
    """Assign thread counts based on row positions."""
    thread_counts = []
    t = [1,2,4,8,16,56]
    for i in range(len(df)):
        thread_counts.append(t[(i // rows_per_thread)])
    df['Threads'] = thread_counts
    return df

def plot_line_graph(df, x_label, y_label, title, output_file):
    """Plot a line graph with multiple series."""
    thread_columns = df.columns[1:]
    plt.figure(figsize=(10, 6))
    print(df)
    
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
    
    # Assign column names
    write_results.columns = ['Optimization Type', 'Threads', 'Bandwidth']
    
    # Clean up the data
    write_results['Optimization Type'] = write_results['Optimization Type'].str.strip()
    write_results['Threads'] = write_results['Threads'].astype(int)
    write_results['Bandwidth'] = write_results['Bandwidth'].astype(float)

    

    # Pivot the DataFrame to reshape it
    pivot_df = write_results.pivot(index='Optimization Type', columns='Threads', values='Bandwidth')

    # Specify the desired order of the rows
    desired_order = [
        'No Optimization',
        'Set Mem to Zero Before Timing',
        'Non-Temporal Writes + Set Mem to Set Mem to Zero Before Timing'
    ]

    # Reorder the rows based on the desired order
    pivot_df = pivot_df.reindex(desired_order)

    # Reorder columns if needed (optional)
    pivot_df = pivot_df.reindex(sorted(pivot_df.columns), axis=1)

    # Save the reshaped DataFrame to a new CSV
    pivot_df.to_csv("../output/reshaped_write_results.csv")

    # Assign column names
    read_results.columns = ['Unroll Loop Size', 'Bandwidth']
    
    # Assign thread counts
    read_results = assign_thread_counts(read_results, rows_per_thread=5)
    
    # Clean up the data
    read_results['Unroll Loop Size'] = read_results['Unroll Loop Size'].astype(int)
    read_results['Threads'] = read_results['Threads'].astype(int)
    read_results['Bandwidth'] = read_results['Bandwidth'].astype(float)
    
    # Pivot and reorder read results
    pivot_read_df = read_results.pivot(index='Unroll Loop Size', columns='Threads', values='Bandwidth')
    
    # Save the reshaped DataFrame to a new CSV
    pivot_read_df.to_csv("../output/reshaped_read_results.csv")


    write_results = load_csv("../output/reshaped_write_results.csv")
    read_results = load_csv("../output/reshaped_read_results.csv")

    write_results =  write_results[1:]
    read_results = read_results[1:]

    
    # Plot line graph for write results
    plot_line_graph(
        df=write_results,
        x_label='Number of Threads (1=1, 2=2, 3=4, 4=8, 5=16, 6=max(56))',
        y_label='Bandwidth (MB/s)',
        title='Bandwidth vs Number of Threads',
        output_file='../output/write_line_graph.png'
    )

    # Plot heatmap for read results
    plot_heatmap(
        df=read_results,
        x_label='Number of Threads (1=1, 2=2, 3=4, 4=8, 5=16, 6=max(56))',
        y_label='Unroll Loop Size',
        title='Heatmap of Bandwidth vs Unroll Loop Size and Threads',
        output_file='../output/read_heatmap.png'
    )

    # Plot heatmap for write results with a custom color map
    plot_heatmap(
        df=write_results,
        x_label='Number of Threads (1=1, 2=2, 3=4, 4=8, 5=16, 6=max(56))',
        y_label='Optimization Type',
        title='Heatmap of Write Bandwidth vs Optimization and Threads',
        output_file='../output/write_heatmap.png',
        cmap=sns.color_palette("coolwarm", as_cmap=True)
    )

if __name__ == "__main__":
    main()