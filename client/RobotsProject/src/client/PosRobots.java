package client;

import java.util.ArrayList;

public class PosRobots {
	
	ArrayList<Integer> pos;
	Couleur c;  //Couleur de la cible
	
	public PosRobots(){          //Position des robots et de la cible
		pos = new ArrayList<>();
	}
	
	public void add(int val){
		pos.add(val);
	}
	
	public void setColor(String color){
		if(color.equals("R"))
			this.c = Couleur.R;
		else if(color.equals("B"))
			this.c = Couleur.B;
		else if(color.equals("J"))
			this.c = Couleur.J;
		else
			this.c = Couleur.V;
		
	}
	
	public ArrayList<Integer> getPositions(){
		return pos;
	}
	
	public Couleur getColor(){
		return c;
	}

}
