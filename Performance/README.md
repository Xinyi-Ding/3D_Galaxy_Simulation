## README - Performance Results

This folder contains performance results stored in CSV files, each representing tests conducted with different configurations for various code versions.

**`analyse.ipynb`**: This Jupyter Notebook is used to generate performance charts from the CSV data, allowing for visual analysis and comparison of the different configurations and code versions.

### Setup for Jupyter Notebook Kernel
1. **Create a Conda virtual environment:**
   ```bash
   conda create --name ipynb python=3.11
   ```
2. **Activate the environment:**
   ```bash
   conda activate ipynb
   ```
3. **Install Jupyter Notebook:**
   ```bash
   conda install -c conda-forge notebook
   ```
4. **Install necessary Python packages:**
   ```bash
   conda install numpy pandas matplotlib
   ```