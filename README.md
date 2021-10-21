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
    The coded heuristics are explained on the published article [refence here]. If you use this code, please cite us on your article! =)
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

The Capacitated Vehicle Routing Problem (CVRP) is a known combinatorial problem for it's difficult NP-hard. We love the Differential Evolution metaheuristic, so we proposed an combinatorial adaptation so we can use DE on combinatorious problems, as well on CVRP! =)

On literature there are other adaptations to use DE on combinatorious problems. Teoh et al. [2] have done a great article using DE for CVRP, which uses a easy to code adaptation for DE, which ranks the real numbers and assign integers considering the ranks of the numbers, where lower number are assigned to lower integers. It's not only easy to code, but also gives good results, but we have proposed an adaptation that use the position of the clients instead which gave us better results. The method is explained detailed on our article [1].

I'm publishing the code to make it easier to reproduce. Sadly on optimization researches it's not a common practice, but let's change that? ;)



The CDELS, which stands for Combinatorial Differential Evolution with Local Search is coded on C language. I didn't think I would get any optimum result at all, so I didn't code it thinking on share the code, so it's not coded on English, sorry... 
I've started learning metaheuristics for a college subject and chosen the Differential Evolution to solve the CVRP, but I loved it and kept studying this project for more than 4 years!
Maybe I will re-write this code to make it more user and reader friendly (in english as well!) and make to change parameters better too, untill that I will try to explain how you can use this version.
 



<!-- GETTING STARTED -->
## Getting Started

First you need a MDVRPTW problem instancy, which you can get on VRP Libraries.
We will list some libraries in which you can get them. We have the "instances" folder with some CVRPLIB instances, mostly for backup purposes but You can use them too!

You can also make your own instance, but it needs to follow TSPLIB standards. 

* [CVRPLIB](http://vrp.atd-lab.inf.puc-rio.br/)


### Prerequisites

gcc and make.


### Installation

1. Change the parameters
   WIP!
   
2. Run the project
   ```sh
   make
   make run
   ```



<!-- USAGE EXAMPLES -->
### Parameters description

WIP!


<!-- CONTACT -->
## Contact

Israel P. - israelpereira55@gmail.com

Project Link: [https://github.com/israelpereira55/CDELS](https://github.com/israelpereira55/CDELS)

[![LinkedIn][linkedin-shield]][linkedin-url]




<!-- ACKNOWLEDGEMENTS -->
## Acknowledgements
* [Best README Template](https://github.com/israelpereira55/MDVRPTW)


## References

[1] Our article reference

[2] Teoh et al reference




<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://www.linkedin.com/in/israel-souza-06737118b/
[product-screenshot]: images/screenshot.png
