# CDELS

<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/israelpereira55/MDVRPTW-Solomon">
    <img src="images/M101k10.jpg" alt="Logo" width="541" height="400">
  </a>

  <h3 align="center">An algorithm based on Differential Evolution with Local Search for the CVRP.</h3>

  <p align="center">
    This is  an algorithm that seeks to get the optimum solutions for the CVRP. 
    <br />
    The coded heuristics are explained on the published article [1]. If you use this code, please cite us in your article or work! =)
    <br />
  </p>
</p>



<!-- TABLE OF CONTENTS -->
<details open="open">
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#setup">Setup</a></li>
      </ul>
    </li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#thanks-to">Acknowledgments</a></li>
    <li><a href="#references">References</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

The Capacitated Vehicle Routing Problem (CVRP) is a known combinatorial problem for its difficulty (NP-hard).

This project started when I chose the Differential Evolution in a metaheuristics competition in a class at the university to solve the CVRP. I got third place, but then started studying it with other two professors for more than four years. 

I love the Differential Evolution metaheuristic. So, we have proposed a combinatorial adaptation for DE for CVRP, that can also be applied to other combinatorial problems.

In the literature, there are several DE adaptations for combinatorial problems. Teoh et al. [2] have done a great article using DE for CVRP, which uses an easy-to-code adaptation for DE proposed by (Mingyong,  L.  and  Erbao,  C), which ranks the real numbers and assigns integers considering the ranks of the numbers, where a lower number are assigned to lower integers. It's not only easy to code but also gives good results. We have proposed an algorithm based in DE with an adaptation for mutation and crossover DE mechanisms. This adaptation uses the position of the clients in chromosomes which presented a better result than arithmetically updating their values. The complete algorithm is explained detailed in our article [1].

I'm publishing the code to make it easier to reproduce. Sadly,  it's not a common practice for papers in the optimization area, but we can change that! ;)

The CDELS stands for Combinatorial Differential Evolution with Local Search. CDELS is coded in C language. I didn't code it thinking to share the code, so it's not coded in English, sorry... 
I have plans to rewrite this code (maybe on C++) to make it more user-friendly and reader-friendly (in English as well!) and also improve the interfaces for parameter decisions but untill that I will explain how you can use this version.
 



<!-- GETTING STARTED -->
## Getting Started

First, you need a CVRP problem instance, which you can get on VRP Libraries.
We will list some libraries in which you can get them. We also have the "instances" folder with some CVRPLIB instances, mostly for backup purposes which are already processed but you can use them too.

You can make your own instance. It just needs to follow TSPLIB95 standards. 

* [CVRPLIB](http://vrp.atd-lab.inf.puc-rio.br/)


You need to clean the header of the instance before using it on CDELS.
The first line should have four integers. The number of vehicles, optimum value of the instance, number of clients
and vehicle capacity. If you don't know the optimum value just put a low value which should be
bellow the lower bound (just write a small integer! ;) ).

**Important:** We are using the TSPLIB95 standard that rounds to the nearest integer the operation cost with floating numbers. If you don't want that, modify the distance matrix creating function.


### Prerequisites

gcc and make.


### Setup

1. Modify the parameters
    * [differential_evolution.h](https://github.com/israelpereira55/CDELS/blob/master/metaheuristic/differential_evolution.h)

      To choose the F, CR, Penality, and MaxGen parameters.


    * [differential_evolution.c](https://github.com/israelpereira55/CDELS/blob/master/metaheuristic/differential_evolution.c)


      To choose a fixed value for the NP parameter, remove the NP definition in the main file.

2. Run the project
   ```sh
   make
   make run
   ```



<!-- USAGE EXAMPLES 
### Parameters description

WIP!
-->

<!-- ACKNOWLEDGEMENTS  -->
## Known bugs

Using very large instances (250+ customers) we noticed in rare cases that the final solution cost using the distance matrix was presenting a slightly different value (1-2 units of difference) than calculating the cost directly, probably due to the use of TSPLIB95 rounding standards and distance matrix. Until we find a way to deal with this scenario, we suggest disabling the distance matrix and always verifying the final solution cost or disabling TSPLIB95 rounding standards if possible. To verify the solution you can use [CVRPPLOT](https://github.com/israelpereira55/CVRPPLOT).

All solutions presented in the article were validated to ensure that they were feasible and had the exact solution cost as reported.

If you find an execution with unexpected results, please report presenting the instance file and the seed to reproduce the problem.

For future, I have plans to reimplement this software for a more user-friendly experience.


<!-- CONTACT -->
## Contact

Project Link: [https://github.com/israelpereira55/CDELS](https://github.com/israelpereira55/CDELS)

[![LinkedIn][linkedin-shield]][linkedin-url]




<!-- ACKNOWLEDGEMENTS  -->
## Acknowledgments

* [Best README Template](https://github.com/othneildrew/Best-README-Template)



## References

[1] Souza, Israel Pereira, Maria Claudia Silva Boeres, and Renato Elias Nunes Moraes. "A robust algorithm based on Differential Evolution with local search for the Capacitated Vehicle Routing Problem." Swarm and Evolutionary Computation (2023): 101245.

[2] Teoh, Boon Ean, Sivalinga Govinda Ponnambalam, and Ganesan Kanagaraj. "Differential evolution algorithm with local search for capacitated vehicle routing problem." International Journal of Bio-Inspired Computation 7.5 (2015): 321-342.

[3] Storn, Rainer, and Kenneth Price. "Differential evolution–a simple and efficient heuristic for global optimization over continuous spaces." Journal of global optimization 11.4 (1997): 341-359.




<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://www.linkedin.com/in/israel-souza-06737118b/
[product-screenshot]: images/screenshot.png
