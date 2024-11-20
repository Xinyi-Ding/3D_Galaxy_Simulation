# Galaxy Exploration: Accessible Galaxy Simulations in Education and Outreach with Wee Archie

## Project Information
- **Project Title:** Parallel Barnes-Hut N-body Toy Galaxy Simulation for Outreach
- **Project Lead:** Xinyi Ding
- **Supervisors:** Darren White, Laura Moran

## Brief Description
This project is dedicated to democratizing the access to galaxy simulations through the development of a scaled-down, parallelized version optimized for the mini-supercomputer, [Wee Archie](https://www.archer2.ac.uk/community/outreach/materials/wee_archie). Our goal is to enhance educational and outreach efforts by making complex astrophysical phenomena more accessible to students and the public.

Challenges arose due to the unavailability of Wee Archie, leading to the use of the Cirrus system for debugging and performance testing. Despite this deviation, the project successfully built a scalable parallel galaxy simulation. Optimal performance was achieved using two MPI processes per node with 18 threads each, and performance improved significantly with more nodes. The simulation effectively scales across up to eight nodes, demonstrating substantial runtime improvements over the serial version.

## Quick Overview of the Simulation Result
![Galaxy Simulation](/visualization/Final_Simulation_Result.gif)

*Figure 1: Simulation of a galaxy with $10^5$ particles with 2000 iterations (The gif image may take some time to load)*

## Repository Overview
This repository serves as the central hub for all code, documentation, and resources related to the "Galaxy Exploration" project. It's structured into various branches and directories, each catering to different aspects of the project's development.

### Main Branch Highlights
The `main` branch is the heart of our project, containing key resources and foundational work. Key features and directories include:

- **[/LiteratureReview](/LiteratureReview-FeasibilityStudy/)**: This directory houses research papers and analyses conducted at the begining of the project. The focus here is primarily on GPU acceleration for galaxy simulations, serving as an initial exploration of the project's development direction. Note that papers related to the feasibility study are not stored here; for those, please refer to the [Feasibility Study Report](/docs/FeasibilityStudyReport.pdf) in the [`docs`](/docs/) folder for further information.
- **[/docs](/docs/)**: This directory includes comprehensive documentation such as the Ethics Assessment Record, Feasibility Study Report, and Dissertation.
  
- **[/Implementation](/Implementation/)**: This folder contains all the development code for implementing the serial and parallel galaxy simulation models. For more details, refer to the [README](/Implementation/README.md) within this folder.
  
- **[/Performance](/Performance/)**: This directory includes all test results and performance analysis related to the project.
  
- **[/Visualization](/visualization/)**: This folder houses the galaxy simulation visualizations, including the simulation data collected and pre-generated simulation videos.

### Feature Branches
- **[galaxy](https://git.ecdf.ed.ac.uk/msc-23-24/s2507921/-/tree/galaxy) branch**:
This branch hosts a clone of Uriot's Galaxy Simulation repository ([Uriot/Galaxy_simulation](https://github.com/angeluriot/Galaxy_simulation)), along with records of setup attempts and modifications made to adapt the simulation for local machine and EIDF VM.

## Communication and Documentation
- **Meeting Minutes**: All meeting minutes are recorded in wiki. For a comprehensive list and detailed accounts of our meetings, please refer to the [Meeting Minutes Directory](https://git.ecdf.ed.ac.uk/msc-23-24/s2507921/-/wikis/MSc-Project-Meeting-Minutes) in our project Wiki.

## License
This project is licensed under the [MIT License](LICENSE) - see the LICENSE file for details.

---

**Last Updated:** 16/08/2024
