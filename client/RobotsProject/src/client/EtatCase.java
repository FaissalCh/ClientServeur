package client;

public class EtatCase {
	
	private boolean haut = false, bas = false, gauche = false, droite = false;

	public boolean isHaut() {  //isHaut renseigne sur le fait qu'il existe un mur en haut de cette case
		return haut;
	}

	public void setHaut(boolean haut) {
		this.haut = haut;
	}

	public boolean isBas() {
		return bas;
	}

	public void setBas(boolean bas) {
		this.bas = bas;
	}

	public boolean isGauche() {
		return gauche;
	}

	public void setGauche(boolean gauche) {
		this.gauche = gauche;
	}

	public boolean isDroite() {
		return droite;
	}

	public void setDroite(boolean droite) {
		this.droite = droite;
	}
	
	

}
