Ising Project
=============

This readme contains instructions for compiling my program, in addition to
passing in instructions to the program via text files, and importing and
exporting images via use of the Magick++ library, which I do not provide owing to
its size. I do provide images of sample spin configurations for this reason.


Getting Started
===============

Please check that the following files are present:

*	IsingModel.cc
*	IsingMatrix.h
*	IsingCell.h
*	parser.h
*	simulator.h

In addition, if you wish to use the additional image functionality (for which you
will need the Magick++ library) you will also need the file imageProcessing.cc

If all these files are present, run any of the following to compile:

*	make
*	make complete
*	make concise

The latter is the only option that does not require the Magick++ library. The
first two are equivalent.

Instruction Files
=================

Basic Syntax
------------

The program, once compiled, takes as a parameter the name of a file. This file
should contain instructions for the simulation(s) to be run, and should be
present in the executable's directory.

Instruction files should be headed with a single line, reading INSTRXN.

Each subsequent line in the instruction file provides the initial conditions for
either one simulation, or a batch of simulations.

Each line in the instruction file should have the following variables, in order:

*	Exchange energy
*	Field
*	Temperature
*	Initial layout
*	Total time

These should be delimited by semi-colons, for a total of 4 semi-colons per line.
Whitespace can be inserted for your own convenience, as the parser will strip it.

Format of Parameters
--------------------

Exchange energy should be a single value (one might as well use either +1 or -1,
since the choice of units sets |J|=1 anyway)

Field should either be a single value, or a 3-tuple written as:
min:max:increment (but see 1.c. for time-varying fields)
The simulation will be run once for each field value defined by the tuple.

Temperature should either be a single value, or a 3-tuple written as:
min:max:increment (but see 1.c. for time-varying temperature)
The simulation will be run once for each temperature value defined by the tuple.

Note that if ranges are specified for both field and temperature, every
combination will be simulated by the program. Beware of making the ranges too
granular when specifying both field and temperature ranges, as this can quickly
lead to a vast number of simulations.

Initial layout should either be:
* 	a 3-tuple of form length:dimension:magnetisation if one wishes to use
	the random layout generator provided in the program
* 	if one wishes to import a layout from a text or image file
	(which should be in the same directory as the executable)
	the filename, with extension, should be provided as follows:

	* image file: i:filename.gif[:threshold]
		* threshold determines which colours are considered spin-up/down
		* if left blank it defaults to 0.5

	* text file: t:filename.dat

Total time should be a single integer - the number of timesteps to be evaluated.

Time Variation
--------------

Time-varying fields and temperature can also be handled:
One should, rather than writing a single value or 3-tuple expressing a range,
write one of the following tuples, which are prefixed with a letter.

Sinusoid: s:amplitude:period:phase
Phase should take any value in the interval [0,1); the function used here is:
amplitude * cos(2pi * ( (t/period)+phase ) )

Polynomial: p:constant term:t-coefficient:t^2-coefficient:[etc.]
As many coefficients as needed should be given.

Exponential: e:initial value:asymptotic value:time constant
Note time constant should be positive for growth, and negative for decay.
The program will work out the correct functional form.

Note that one timestep is treated as the unit of time.

If piecewise functions are desired (for example, a tophat function), one should
simulate each piece in turn, using the final layout of one piece as the initial
layout for the next.

Layout Files
============

One can also use text files to pass in a specific layout. When using a text file
to represent an initial layout, the following format should be followed.

Layout files should be headed with a single line, reading LAYOUT:<n>:<d>
where <n> and <d> are the length and dimension of the Ising lattice. The colons
are necessary.

On a new line, the contents of the initial layout should be written. This method
allows for two different spin values - +1 ("up") and -1 ("down"). The order spins
are written in is the order they'll be generated in, and this will affect which
spins neighbour which.

Up spins can be represented by any of the following characters: +, 1, or u
Down spins can be represented by any of the following characters: -, 0, or d

Whitespace can be inserted for your own convenience, as the parser will strip it.
I recommend you make liberal use of whitespace here, to avoid making mistakes.

The program will expect N^D spin values to be provided. If you do not provide
enough, then down spins will be appended automatically by the program. Surplus
spin values will be discarded.

If you wish to specify a repeating pattern of spins (for example, a checkerboard
pattern), append a tilde ~ to the end of the repeating pattern.
The given example of a checkerboard layout would be then: 10~ or 01~ (or similar)

Image Functionality
===================

If you choose to compile the complete version, having installed the Magick++
library, then you will notice you can also import and export images. Importing
images to pass in an initial layout is discussed in section 2.b.

The program will automatically export three images per simulation upon
completion:

 *	IsingInit<n>.gif, a static gif of the initial layout
 *	IsingLast<n>.gif, a static gif of the final layout
 *	IsingAnim<n>.gif, an animated gif charting the evolution of the system