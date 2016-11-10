# VBFNLO

# try ./run.sh to see how to run the process.

Arrangement
------------
    The tools are written in such a consideration: try to divide the process into different part, and them connect them with bash script to get 
most flexibility.

Fiducial:   extract those kinematic variables that we are interested in from the original particle info. You can add other variables you want to	      investigate into here. 
Raw:	    similar with Fiducial, but with unsorted leptons and Z, allow one to check if the production process work as expect.
cut;	    Cut the the Fiducial results, you can applied other cut you are interested in here.


Tools
-----
*.cxx   Root script to extract kinematic and cut on them
*.py    Python script to draw better kinematic distributon


leptons pairs strategy
---------------------
    for eeμμ, it's easy to pair them to get Z(ee) and Z(μμ); 
    for eeee or μμμμ channel, I choose the two opposite charged lepton pairs with smallest sum of mass difference with M(Z):
	       | M₁(ll) - M(Z) | + | M₂(ll) - M(Z) |

Improtant Variables
-------------------
Rapidity:    Y = ½∙log( (E + Pz) / (E - Pz) )
ΔR:	     ΔR(l₁l₂) = √( (η(l₁) - η(l₂))² + (φ(l₁) - φ(l₂))²)
centrality:  C = ( Y(llll) - (Y(j₁) + Y(j₂))/2 ) / abs(Y(j₁) - Y(j₂)) 


