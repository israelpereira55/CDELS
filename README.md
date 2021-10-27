# CDELS

<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/israelpereira55/MDVRPTW-Solomon">
    <img src="images/mona-lisa100K.gif" alt="Logo" width="500" height="500">
  </a>

  <h3 align="center">An algorithm based on Differential Evolution with Local Search for the CVRP.</h3>

  <p align="center">
    This is  an algorithm that seeks to get the optimum solutions for the CVRP. 
    <br />
    The coded heuristics are explained on the published article [refence here]. If you use this code, please cite us in your article! =)
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
        <li><a href="#installation">Installation</a></li>
        <li><a href="#flags-description">Flags description</a></li>
      </ul>
    </li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgements">Acknowledgements</a></li>
    <li><a href="#references">References</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

The Capacitated Vehicle Routing Problem (CVRP) is a known combinatorial problem for it's difficult (NP-hard). We love the Differential Evolution metaheuristic, so we have proposed a combinatorial adaptation so we can use DE on combinatorial problems, as well on CVRP! =)

On literature there are other adaptations to use DE on combinatorial problems. Teoh et al. [2] have done a great article using DE for CVRP, which uses a easy to code adaptation for DE proposed by (Mingyong,  L.  and  Erbao,  C), which ranks the real numbers and assign integers considering the ranks of the numbers, where lower number are assigned to lower integers. It's not only easy to code, but also gives good results, but we have proposed an adaptation that use the position of the clients instead which gave us better results. The method is explained detailed on our article [1].

I'm publishing the code to make it easier to reproduce. Sadly on optimization researches it's not a common practice, but let's change that? ;)



The CDELS, which stands for Combinatorial Differential Evolution with Local Search is coded on C language. I didn't think I would get any optimum result at all, so I didn't code it thinking on sharing the code, so it's not coded on English, sorry... 
I've started learning metaheuristics for a college subject and chosen the Differential Evolution to solve the CVRP, but I loved it and kept studying this project for more than 4 years!
Maybe I will re-write this code to make it more user and reader friendly (in english as well!) and make to change parameters better too, untill that I will try to explain how you can use this version.
 



<!-- GETTING STARTED -->
## Getting Started

First you need a CVRP problem instancy, which you can get on VRP Libraries.
We will list some libraries in which you can get them. We have the "instances" folder with some CVRPLIB instances, mostly for backup purposes which are already processes and You can use them too!

You can also make your own instance, but it needs to follow TSPLIB standards. 

* [CVRPLIB](http://vrp.atd-lab.inf.puc-rio.br/)


You need to clean the header of the instance before using it on CDELS.
The first line should have 4 integers. The number of vehicles, optimum value of the instance, number of clients
and vehicle capacity. If you don't know the instancy's optimum value, then just put a low value which should be
bellow the lower bound (just write a small integer! ;) ). Then put the instance on the same path of the executable and you can run it.

**Important:** We are using the TSPLIB95 standard which rounds any operation that use floating numbers. If you don't want that, change the distance matrix.


### Prerequisites

gcc and make.


### Setup

1. Change the parameters
    * [EvolucaoDiferencial.h](https://github.com/israelpereira55/CDELS/blob/main/HeadED/EvolucaoDiferencial.h)
      To change F, CR, Penality and MaxGen.

    * [EvolucaoDiferencial.c](https://github.com/israelpereira55/CDELS/tree/main/Combinatoria)
      To change NP. You can use it statically with the header if you want, but on this case you need to remove the NP definition on the main file.

2. Run the project
   ```sh
   make
   make run
   ```



<!-- USAGE EXAMPLES 
### Parameters description

WIP!
-->

<!-- CONTACT -->
## Contact

Israel P. - israelpereira55@gmail.com

Project Link: [https://github.com/israelpereira55/CDELS](https://github.com/israelpereira55/CDELS)

[![LinkedIn][linkedin-shield]][linkedin-url]




<!-- ACKNOWLEDGEMENTS  -->
## Thanks to
* Storn and Price for Differential Evolution

* [math.uwaterloo](https://www.math.uwaterloo.ca/tsp/index.html) for the amazing mona lisa picture and the challenge. One day I should try it! =D

* [Best README Template](https://github.com/othneildrew/Best-README-Template)



## References

[1] Our article reference

[2] Teoh, Boon Ean, Sivalinga Govinda Ponnambalam, and Ganesan Kanagaraj. "Differential evolution algorithm with local search for capacitated vehicle routing problem." International Journal of Bio-Inspired Computation 7.5 (2015): 321-342.

[3] Storn, Rainer, and Kenneth Price. "Differential evolution–a simple and efficient heuristic for global optimization over continuous spaces." Journal of global optimization 11.4 (1997): 341-359.




<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://www.linkedin.com/in/israel-souza-06737118b/
[product-screenshot]: images/screenshot.png
