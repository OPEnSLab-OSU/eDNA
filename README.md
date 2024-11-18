# eDNA Sampler | [Updates](https://github.com/OPEnSLab-OSU/OPEnS-Lab-Home/wiki/eDNA-updates) | [Senior Design Updates](https://github.com/OPEnSLab-OSU/OPEnS-Lab-Home/wiki/eDNA-Senior-Design-Updates) | [Github](https://github.com/OPEnSLab-OSU/eDNA) | [ednaUI](https://github.com/OPEnSLab-OSU/ednaUI) | [ednaServer](https://github.com/OPEnSLab-OSU/ednaServer)

Current Project Team:

Lead: **Marc Belinga** 

Mechanical: **Hendy Kurniawan**

Software:**Marc Belinga** 

Electrical: **Reece Fitch**



## Overview

Organisms leave traces of DNA as they move through their environments. The extraction of these DNA traces is known as environmental DNA (eDNA). eDNA provides scientists and researchers with a non-invasive and sensitive way to detect and quantify species. However, traditional eDNA sampling consists of manually filtering water, which is labor and cost-intensive for remote locations. Furthermore, commercialized solutions are expensive and require a field operator. This eDNA sampler project aims to provide an affordable, open-sourced, remotely deployable, fully automated, and customizable alternative. 

## Description

The eDNA Sampler can run up to 24 inline filter units with support for different conditions including pressure, time, and volume limit. The pumps can deliver a maximum of 350mL/min of flow with each solenoid valve separating the filter housings to minimize cross-contamination. At the end of each sample, the desired stabilizing solution can be injected to fully submerge the filter for long-term storage. An optional river depth sensor can provide a proxy for flow to correct eDNA concentrations to allow for improved quantification of organisms. Data acquired during operation including water depth, pressure, temperature, and flow rate will be stored on a microSD card as a CSV file, which allows easier data export and analysis. A web application is specially designed for the eDNA Sampler for in-field programming, real-time sensor updates, scheduling tasks, and manual operations. The full sampler is estimated to be ~$6000 each.

<p align="center">
  <img src="https://github.com/OPEnSLab-OSU/eDNA/blob/master/Assets/V4eDNASampler.jpg"  width="90%" />
</p>


## Field Updates
The eDNA sampler was successfully deployed in Alaska, where it operated under varying levels of turbidity and successfully collected DNA across all conditions. Over the course of six days, the sampler consistently performed three sample collections per day at multiple locations, showcasing its reliability in diverse environments. The battery capacity proved sufficient for multiple days of operation without requiring a recharge, enabling extended fieldwork in remote areas. Additionally, the sampler achieved an average margin of error of 9% in measuring water volume, demonstrating robust performance while highlighting areas for further refinement.

## Objectives

Determining the level of cross-contamination between filters during sampling.
Testing alternatives to ethanol, such as Longmire's solution, for safer and more convenient preservation during field deployments.
Improving the accuracy of water volume measurements using a tipping bucket mechanism.



| Main Control Board     |
|:-------------:|
| <img src="https://github.com/OPEnSLab-OSU/eDNA/blob/master/Assets/eDNABoard.jpg" width="90%">     | 

| Browser Application     |
|:-------------:|
| <img src="https://user-images.githubusercontent.com/20134802/123329779-079f8100-d4f2-11eb-9892-1380ae95aba6.png" width="90%" /> |

## Future

Finalizing tests for Longmire’s solution to replace ethanol as the preservative.
Implementing a tipping bucket mechanism to improve measurement accuracy.


## Resource List

*  [A Build Guide for the current version (V4) of the eDNA Sampler](https://www.overleaf.com/read/rsvcsjwgfrgj) (Work in Progress)
*  [How to update an assembly in Fusion360](https://www.youtube.com/watch?v=EKoYnxrryUQ)

<!---*  [eDNA GitHub](https://github.com/OPEnSLab-OSU/eDNA)

## [eDNA Updates](https://github.com/OPEnSLab-OSU/OPEnS-Lab-Home/wiki/eDNA-updates) | [Github](https://github.com/OPEnSLab-OSU/eDNA) -->

## Problems & Solutions

Identified logistical challenges with ethanol preservation during field deployment, prompting the exploration of Longmire's solution as an alternative.
While we did average a 9% error in water volume measurements plans are currently being put in place to integrate a tipping bucket to further improve accuracy.
Continued troubleshooting and refinement to reduce motor noise, improve user interface responsiveness, and address filter clogging issues.

Feel free to contact the lab/developer if you have other ideas to solve, improve, or upgrade the eDNA sampler.

Marc Belinga: belingam@oregonstate.edu

## Keywords
eDNA, genomic, sampling, sampler, sample, environmental DNA

## Former Team Members:
Electrical: **Bao Nguyen**, **Sean Booth**,**Jacob Field**, **Kai Roy**

Programming: **Kawin Pechetratanapanit**, **Emannuel Moncada** 

Mechanical: **Torrey Menne**, **Aaron S Arvidson**, **Nikhil Wandhekar**, **Ethan Pohlschneider**, **Tilford Li**, **Danil Kryuchkov**, **Dylan Heisey**, **Joshua Iler**


## Reference
* [Types of Hydraulic Pumps](https://www.youtube.com/watch?v=Qy1iV6EzNHg)

* [eDNA AGU 2019 Poster](https://github.com/OPEnSLab-OSU/OPEnS-Hypnos/files/3996793/eDNA.poster_jsspptx.2.pdf) | [ESSOAR](https://www.essoar.org/doi/abs/10.1002/essoar.10501740.1) | 10.1002/essoar.10501740.1

*  [Inline Pressure Sensor](https://github.com/OPEnSLab-OSU/OPEnS-Lab-Home/wiki/Inline-Pressure-Sensor) (Outdated)

*  [Inline Flow Sensor](https://github.com/OPEnSLab-OSU/OPEnS-Lab-Home/wiki/IDT-Inline-Flow-Sensor) (Outdated)

