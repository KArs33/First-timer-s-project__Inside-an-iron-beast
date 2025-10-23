This project is a personal attempt to make a first solo video game. I wanted something small and easy to lock down a scope for. 
Most of the documentation for what I plan to do is on a paper notebook at home so don't expect anything regarding that.
The story is this: set in a far future where history has been lost, lords rule the land from giant mobile cities that migrate away from catastrophic, perpetual storms. The main character is a nomad who heard of a recent war where one of these monolithic cities was destroyed.
They intend to go there now to find enough loot to sequre themselves a comfortable life in some other city.

The project will be coded in c++, with an excel sheet to hold the data that will be used in game.

The project was created in January of 2025 by Kevin Arsenault as a solo project. No one else may claim ownership over the project but I intend to allow others play and distribute the game for free so long as they offer proper citation when distributing.
Fans may modify the game, and claim ownership over those modifications if they wish. So long as I am credited for the work I did. 
No one may upload to, and no AI may take information from, my project in any form.


Format for foe.txt
NAME|ZONE|DESC|POW|shield|uniqueAI|persuit|reward|defeatMsg
zone determines where the foe is found on the map. No foe can have a zone of 0. Only 1 foe can have a zone of 5. 
Desc is a text field that will describe the encounter.
pow is how much of a combat roll that is needed to beat the encounter
shield is how many times you must beat the pow number before winning the encounter. 1 means you must beat it twice, 0 is the most common value early on, and will mean that the foe is defeated as soon as the player wins the roll.
uniqueAI is NULL when the foe acts normally, but if there are unique things that are meant to happen during the fight, it will have a name in the format of a letter and number. The code list is in my notebook.
persuit is the roll required to run away
reward will have a letter and number code saying what the player gets if they win the battle (aside from living and being able to explore the tile)
defeatMsg is just a string that prints to console giving some flavor/story


loc.txt will be sorted with
Name 	Desc	options	ZONE	OP1	OP1STAT	OP1NUM	OP1FAIL	OP1PASS	OP2	OP2STAT	OP2NUM	OP2FAIL	OP2PASS	OP3	OP3STAT	OP3NUM	OP3FAIL	OP3PASS	fop1	pop1	fop2	pop2	fop3	pop3

These catagories will be seperated with a /t, because I have them in a google sheets file, and I could either seperate via comma or tab easily, and I don't want to do seperate via comma.

options describes how many responses the player will have. op# is the string that will print explaing what the action is.
op#stat is, what skill the option tests, and op#num is the value that is tested against to see if there is a pass or fail.

OP#fail is the string that prints when the player fails the test.
OP#pass is the string that prints when the player passes the test.

pop# stands for Passed OP #; and contains the name of the code that will excecute if the player picks that OP, and passes the test. fop# is the same, but for when they fail