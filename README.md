# eDNA Sampler | [Updates](https://github.com/OPEnSLab-OSU/OPEnS-Lab-Home/wiki/eDNA-updates) | [Senior Design Updates](https://github.com/OPEnSLab-OSU/OPEnS-Lab-Home/wiki/eDNA-Senior-Design-Updates) | [Github](https://github.com/OPEnSLab-OSU/eDNA) | [ednaUI](https://github.com/OPEnSLab-OSU/ednaUI) | [ednaServer](https://github.com/OPEnSLab-OSU/ednaServer)

Current Project team:

Lead: **Riley Prince** 

Mechanical: **Joshua Iler**, **Hendy**

Software: **Nathan Jesudason**, **Marc Belinga** 

Electrical: **Kai Roy**, **Jacob Field**


## Overview

Organisms leave traces of DNA as they move through their environments. The extraction of these DNA traces is known as environmental DNA (eDNA). eDNA provides scientists and researchers with a non-invasive and sensitive way to detect and quantify species. However, traditional eDNA sampling consists of manually filtering water, which is labor and cost-intensive for remote locations. Furthermore, commercialized solutions are expensive and require a field operator. This eDNA sampler project aims to provide an affordable, open-sourced, remotely deployable, fully automated, and customizable alternative. 

## Description

The eDNA Sampler can run up to 24 inline filter units with support for different conditions including pressure, time and volume limit. The pumps can deliver maximum 350mL/min of flow with solenoid valves separating each inline filter to minimize cross-contamination. At the end of each sample, the desired stabilizing solution can be injected to fully submerge the filter for long term storage. An optional river depth sensor can provide a proxy for flow to correct eDNA concentrations to allow for improved quantification of organisms. Data acquired during operation including water depth, pressure, temperature, and flow rate will be stored on microSD card as a CSV file, which allows easier data export and analysis. A web application is specially designed for the eDNA Sampler for in-field programming, real-time sensor updates, scheduling tasks, and manual operations. The full sampler is estimated to be ~$5000 each, with add-on river depth sensor and 10ah 12V battery.

<p align="center">
  <img src="https://github.com/OPEnSLab-OSU/eDNA/blob/master/Assets/V4eDNASampler.jpg"  width="90%" />
</p>

## Objectives

*  Depth Sensor

*  [Inline Pressure Sensor](https://github.com/OPEnSLab-OSU/OPEnS-Lab-Home/wiki/Inline-Pressure-Sensor) (Outdated)

*  [Inline Flow Sensor](https://github.com/OPEnSLab-OSU/OPEnS-Lab-Home/wiki/IDT-Inline-Flow-Sensor) (Outdated)

## Outcomes

We are currently working on the electronic hardware and software side of the project. More test will be performed on the system as a whole to determine the level of cross-contamination between different filter.

| Main control Board     |
|:-------------:|
| <img src="https://github.com/OPEnSLab-OSU/eDNA/blob/master/Assets/eDNABoard.jpg" width="90%">     | 
| Brand New Browser Application     |
| <img src="https://user-images.githubusercontent.com/20134802/123329779-079f8100-d4f2-11eb-9892-1380ae95aba6.png" width="90%" /> |

## Future
The main things that we are currently working on are: 
 - Determining the cross contamination between filters (samples).
   - Brainstorming ways to increase cross contamination if it falls below desired margins.
 - Implementing a better way of monitoring the amount of volume sampled.

In addition to the previously mentioned tasks, we are planning to work on the following in the near future:
 - Redesign of the main control board/electronics hardware.
 - Making the User Interface more friendly to less technical users.
 - Decreasing the amount of cross contamination between samples (depends on the testing currently happening).


## Resource List

*  [A Build Guide for the current version (V4) of the eDNA Sampler](https://www.overleaf.com/read/rsvcsjwgfrgj) (Work in Progress)
*  [How to update an assembly in Fusion360](https://www.youtube.com/watch?v=EKoYnxrryUQ)

<!---*  [eDNA GitHub](https://github.com/OPEnSLab-OSU/eDNA)

## [eDNA Updates](https://github.com/OPEnSLab-OSU/OPEnS-Lab-Home/wiki/eDNA-updates) | [Github](https://github.com/OPEnSLab-OSU/eDNA) -->

## Problems & Solutions

What problem we have encountered in the development of the series and the solution that we have come up with:

*  [Reducing motor noise](https://github.com/OPEnSLab-OSU/OPEnS-Lab-Home/wiki/Reducing-motor-noise)

*  Alcohol retention for unclogged filter

*  Sending large website package

*  [Troubleshoot?](https://github.com/OPEnSLab-OSU/OPEnS-Lab-Home/wiki/eDNA-Troubleshoot)

Feel free to contact the lab/developer if you have other ideas to solve, improve, or upgrade the eDNA sampler.

Riley Prince: princeri@oregonstate.edu

## Keywords
eDNA, genomic, sampling, sampler, sample, environmental DNA

## Former Team Members:
Electrical: **Bao Nguyen**, **Sean Booth**

Programming: **Kawin Pechetratanapanit**, **Emannuel Moncada** 

Mechanical: **Torrey Menne**, **Aaron S Arvidson**, **Nikhil Wandhekar**, **Ethan Pohlschneider**, **Tilford Li**, **Danil Kryuchkov**, **Dylan Heisey**


## Reference
* [Types of Hydraulic Pumps](https://www.youtube.com/watch?v=Qy1iV6EzNHg)

* [eDNA AGU 2019 Poster](https://github.com/OPEnSLab-OSU/OPEnS-Hypnos/files/3996793/eDNA.poster_jsspptx.2.pdf) | [ESSOAR](https://www.essoar.org/doi/abs/10.1002/essoar.10501740.1) | 10.1002/essoar.10501740.1

