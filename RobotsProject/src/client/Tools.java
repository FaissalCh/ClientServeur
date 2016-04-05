package client;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.util.ArrayList;

public class Tools {

	/*Recuperer l'etat de chaque case c'est-a-dire sa position et les murs la bordant*/
	public static EtatCase[][] getPlateau(String line) throws NumberFormatException, IOException{

		String mur = "", murX = "", murY = "", direction;
		EtatCase cases[][] = new EtatCase[16][16];
		for(int i = 0; i < 16; i++){
			for(int j = 0; j < 16; j++){
				cases[i][j] = new EtatCase();
			}
		}

		while( !line.isEmpty() && ! line.equals(")")){

			String cur = "";
			int k = 0;
			while( !cur.equals(")")){
				mur += cur;
				cur = line.charAt(k)+"";
				k++;
			}

			cur = "";
			int i = 1;
			while( !cur.equals(",")){
				murX += mur.charAt(i);
				i++;
				cur = mur.charAt(i)+"";
			}

			cur = "";
			int j = i+1;
			while( !cur.equals(",")){
				murY += mur.charAt(j)+"";
				j++;
				cur = mur.charAt(j)+"";
			}

			direction = mur.charAt(j+1)+"";

			if(direction.equals("H")){
				cases[Integer.valueOf(murX)][Integer.valueOf(murY)].setHaut(true);
			}
			else if(direction.equals("B"))
				cases[Integer.valueOf(murX)][Integer.valueOf(murY)].setBas(true);
			else if(direction.equals("G"))
				cases[Integer.valueOf(murX)][Integer.valueOf(murY)].setGauche(true);
			else if(direction.equals("D"))
				cases[Integer.valueOf(murX)][Integer.valueOf(murY)].setDroite(true);
			line = line.substring(j+3, line.length());
			murX = "";
			murY = "";
			mur = "";
		}
		return cases;

	}


	public static String signalerConnexion(Socket c, String userName) throws IOException{

		OutputStream os = c.getOutputStream();
		BufferedWriter br = new BufferedWriter(new OutputStreamWriter(os));
		br.write("CONNEXION/" + userName + "/\n");
		br.flush();

		InputStream is = c.getInputStream();
		@SuppressWarnings("resource")
		InputStreamLiner isl = new InputStreamLiner(is);
		String line = isl.readLine(InputStreamLiner.UNIX);
		
		return line;
	}


	public static String signalerConnexionPrivee(Socket c, String session, String userName,
			String pw, boolean createSession) throws IOException {
		OutputStream os = c.getOutputStream();
		BufferedWriter br = new BufferedWriter(new OutputStreamWriter(os));
		if(createSession)
			br.write("CREERSESSION/" + session + "/" + pw + "/" + userName + "/\n");
		else
			br.write("CONNEXIONPRIVEE/" + session + "/" + pw + "/" + userName + "/\n");
		br.flush();

		InputStream is = c.getInputStream();
		@SuppressWarnings("resource")
		InputStreamLiner isl = new InputStreamLiner(is);
		String line = isl.readLine(InputStreamLiner.UNIX);

		return line;
	}


	/*Recuperer position des robots et de la cible avec sa couleur*/
	public static PosRobots getRobots(String line) throws IOException {

		PosRobots retour = new PosRobots();
		String cur_number = "";
		int cpt = 0;

		while( !line.isEmpty() ){
			int i = 0;
			while( (!(line.charAt(i)+"").equals(","))){
				cur_number += line.charAt(i);
				i++;
				if(line.length() <= i )
					break;
			}
			if(cpt < 10){
				retour.add(Integer.valueOf(cur_number));
				cpt++;
			}else{
				retour.setColor(cur_number);
				break;
			}
			line = line.substring(i+1, line.length());
			cur_number = "";
		}

		return retour;
	}


	/*Solution a la phase de resolution*/
	public static void sendSolution(Socket c, String user, String text) throws IOException {
		OutputStream os = c.getOutputStream();
		BufferedWriter br = new BufferedWriter(new OutputStreamWriter(os));
		br.write("SOLUTION/" + user + "/" + text.toUpperCase()+"\n");
		br.flush();
	}


	/*Solution a la phase de reflexion*/
	public static void sendNbMoves(Socket c, String user, String text) throws IOException {
		OutputStream os = c.getOutputStream();
		BufferedWriter br = new BufferedWriter(new OutputStreamWriter(os));
		br.write("SOLUTION/" + user + "/" + text+"/\n");
		br.flush();
	}



	public static void deconnexion(Socket c, String user) throws IOException {
		OutputStream os = c.getOutputStream();
		BufferedWriter br = new BufferedWriter(new OutputStreamWriter(os));
		br.write("SORT/" + user + "/\n");
		br.flush();
	}



	public static String bilan(Socket c) throws IOException {
		InputStream is = c.getInputStream();
		@SuppressWarnings("resource")
		InputStreamLiner isl = new InputStreamLiner(is);
		return isl.readLine(InputStreamLiner.UNIX);
	}



	public static void sendEnchere(Socket c, String user, String text) throws IOException {
		OutputStream os = c.getOutputStream();
		BufferedWriter br = new BufferedWriter(new OutputStreamWriter(os));
		br.write("ENCHERE/" + user + "/" + text + "/\n");
		br.flush();
	}



	/*Message de Chat*/
	public static void sendMessage(Socket c, String user, String text) throws IOException {
		OutputStream os = c.getOutputStream();
		BufferedWriter br = new BufferedWriter(new OutputStreamWriter(os));
		br.write("CHAT/" + user + "/" + text + "/\n");
		br.flush();
	}



	/*Verifie que le nombre de coups est non nul et est un chiffre*/
	public static boolean isNumericAndNotZero(String text) {
		String pattern= "^[0-9]*$";
		String patternZero= "^[0]*$";
		if(text.matches(pattern) && !text.matches(patternZero)){
			return true;
		}
		return false; 
	}



	/*Verifie que la solution saisie contient des couples (robot, direction)*/
	public static boolean isCorrectMove(String text) {

		if(text.length() < 2 || (text.length()%2 != 0))   //Il faut qu a chaque robot corresponde une direction
			return false;

		while(!text.isEmpty()){
			char a = text.charAt(0);
			char b = text.charAt(1);
			if(a != 'R' && a != 'B' && a != 'J' && a != 'V')
				return false;
			if(b != 'H' && b != 'B' && b != 'G' && b != 'D')
				return false;
			text = text.substring(2);
		}
		return true;
	}



	/***Methode servant a retourner une solution de la forme (robot, direction, nbCase)*  ****/
	/*Utile car il faut a un moment donne calculer le trajet d'un robot lorsqu'il se deplace selon une direction (savoir de combien de cases)*/
	public static String getSolution(String sol, ArrayList<Integer> liste, EtatCase[][] cases, Couleur color) {

		String tmp = "";
		int deplX = 0, deplY = 0;     //deplacement du pion
		String robot, direction;
		int posXRed = liste.get(0), posYRed = liste.get(1);   //Position actuelle des robots
		int posXBlue = liste.get(2), posYBlue = liste.get(3);
		int posXYellow = liste.get(4), posYYellow = liste.get(5);
		int posXGreen = liste.get(6), posYGreen = liste.get(7);
		int posXCible = liste.get(8), posYCible = liste.get(9);
		String colorCible;
		if(color == Couleur.R)
			colorCible = "R";
		else if(color == Couleur.B)
			colorCible = "B";
		else if(color == Couleur.J)
			colorCible = "J";
		else
			colorCible = "V";

		while(!sol.isEmpty()){

			robot = sol.charAt(0)+"";
			direction = sol.charAt(1)+"";

			if(direction.equals("H")){
				if(robot.equals("R")){
					while(!cases[posXRed][posYRed].isHaut()){
						if(posYRed == 0)
							break;
						if(!cases[posXRed][posYRed-1].isBas() && !(posXRed == posXBlue && posYRed-1 == posYBlue)
								&& !(posXRed == posXYellow && posYRed-1 == posYYellow) && !(posXRed == posXGreen && posYRed-1 == posYGreen)){
							deplY++;
							posYRed--;
							if(posXRed == posXCible && posYRed == posYCible && colorCible.equals("R"))
								break;
						}
						else
							break;
					}
				}

				else if(robot.equals("B")){
					while(!cases[posXBlue][posYBlue].isHaut()){
						if(posYBlue == 0)
							break;
						if(!cases[posXBlue][posYBlue-1].isBas() && !(posXBlue == posXRed && posYBlue-1 == posYRed)
								&& !(posXBlue == posXYellow && posYBlue-1 == posYYellow) && !(posXBlue == posXGreen && posYBlue-1 == posYGreen)){
							deplY++;
							posYBlue--;
							if(posXBlue == posXCible && posYBlue == posYCible && colorCible.equals("B"))
								break;
						}
						else
							break;
					}
				}

				else if(robot.equals("J")){
					while(!cases[posXYellow][posYYellow].isHaut()){
						if(posYYellow == 0)
							break;
						if(!cases[posXYellow][posYYellow-1].isBas() && !(posXYellow == posXRed && posYYellow-1 == posYRed)
								&& !(posXBlue == posXYellow && posYBlue == posYYellow-1) && !(posXYellow == posXGreen && posYYellow-1 == posYGreen)){
							deplY++;
							posYYellow--;
							if(posXYellow == posXCible && posYYellow == posYCible && colorCible.equals("J"))
								break;
						}
						else
							break;
					}
				}
				else {
					while(!cases[posXGreen][posYGreen].isHaut()){
						if(posYGreen == 0)
							break;
						if(!cases[posXGreen][posYGreen-1].isBas() && !(posXGreen == posXRed && posYGreen-1 == posYRed)
								&& !(posXGreen == posXYellow && posYGreen-1 == posYYellow) && !(posXBlue == posXGreen && posYBlue == posYGreen-1)){
							deplY++;
							posYGreen--;
							if(posXGreen == posXCible && posYGreen == posYCible && colorCible.equals("V"))
								break;
						}
						else
							break;
					}
				}

			}

			else if(direction.equals("B")){
				if(robot.equals("R")){
					while(!cases[posXRed][posYRed].isBas()){
						if(posYRed == cases[0].length-1)
							break;
						if(!cases[posXRed][posYRed+1].isHaut() && !(posXRed == posXBlue && posYRed+1 == posYBlue)
								&& !(posXRed == posXYellow && posYRed+1 == posYYellow) && !(posXRed == posXGreen && posYRed+1 == posYGreen)){
							deplY++;
							posYRed++;
							if(posXRed == posXCible && posYRed == posYCible && colorCible.equals("R"))
								break;
						}
						else
							break;
					}
				}
				else if(robot.equals("B")){
					while(!cases[posXBlue][posYBlue].isBas()){
						if(posYBlue == cases[0].length-1)
							break;
						if(!cases[posXBlue][posYBlue+1].isHaut() && !(posXBlue == posXRed && posYBlue+1 == posYRed)
								&& !(posXBlue == posXYellow && posYBlue+1 == posYYellow) && !(posXBlue == posXGreen && posYBlue+1 == posYGreen)){
							deplY++;
							posYBlue++;
							if(posXBlue == posXCible && posYBlue == posYCible && colorCible.equals("B"))
								break;
						}
						else
							break;
					}
				}
				else if(robot.equals("J")){
					while(!cases[posXYellow][posYYellow].isBas()){
						if(posYYellow == cases[0].length-1)
							break;
						if(!cases[posXYellow][posYYellow+1].isHaut() && !(posXYellow == posXRed && posYYellow+1 == posYRed)
								&& !(posXBlue == posXYellow && posYBlue == posYYellow+1) && !(posXYellow == posXGreen && posYYellow+1 == posYGreen)){
							deplY++;
							posYYellow++;
							if(posXYellow == posXCible && posYYellow == posYCible && colorCible.equals("J"))
								break;
						}
						else
							break;
					}
				}
				else {
					while(!cases[posXGreen][posYGreen].isBas()){
						if(posYGreen == cases[0].length-1)
							break;
						if(!cases[posXGreen][posYGreen+1].isHaut() && !(posXGreen == posXRed && posYGreen+1 == posYRed)
								&& !(posXGreen == posXYellow && posYGreen+1 == posYYellow) && !(posXBlue == posXGreen && posYBlue == posYGreen+1)){
							deplY++;
							posYGreen++;
							if(posXGreen == posXCible && posYGreen == posYCible && colorCible.equals("V"))
								break;
						}
						else
							break;
					}
				}
			}

			else if(direction.equals("G")){
				if(robot.equals("R")){
					while(!cases[posXRed][posYRed].isGauche()){
						if(posXRed == 0)
							break;
						if(!cases[posXRed-1][posYRed].isDroite() && !(posXRed-1 == posXBlue && posYRed == posYBlue)
								&& !(posXRed-1 == posXYellow && posYRed == posYYellow) && !(posXRed-1 == posXGreen && posYRed == posYGreen)){
							deplX++;
							posXRed--;
							if(posXRed == posXCible && posYRed == posYCible && colorCible.equals("R"))
								break;
						}
						else
							break;
					}
				}
				else if(robot.equals("B")){
					while(!cases[posXBlue][posYBlue].isGauche()){
						if(posXBlue == 0)
							break;
						if(!cases[posXBlue-1][posYBlue].isDroite() && !(posXBlue-1 == posXRed && posYBlue == posYRed)
								&& !(posXBlue-1 == posXYellow && posYBlue == posYYellow) && !(posXBlue-1 == posXGreen && posYBlue == posYGreen)){
							deplX++;
							posXBlue--;
							if(posXBlue == posXCible && posYBlue == posYCible && colorCible.equals("B"))
								break;
						}
						else
							break;
					}
				}
				else if(robot.equals("J")){
					while(!cases[posXYellow][posYYellow].isGauche()){
						if(posXYellow == 0)
							break;
						if(!cases[posXYellow-1][posYYellow].isDroite() && !(posXYellow-1 == posXRed && posYYellow == posYRed)
								&& !(posXBlue == posXYellow-1 && posYBlue == posYYellow) && !(posXYellow-1 == posXGreen && posYYellow == posYGreen)){
							deplX++;
							posXYellow--;
							if(posXYellow == posXCible && posYYellow == posYCible && colorCible.equals("J"))
								break;
						}
						else
							break;
					}
				}
				else {
					while(!cases[posXGreen][posYGreen].isGauche()){
						if(posXGreen == 0)
							break;
						if(!cases[posXGreen-1][posYGreen].isDroite() && !(posXGreen-1 == posXRed && posYGreen == posYRed)
								&& !(posXGreen-1 == posXYellow && posYGreen == posYYellow) && !(posXBlue == posXGreen-1 && posYBlue == posYGreen)){
							deplX++;
							posXGreen--;
							if(posXGreen == posXCible && posYGreen == posYCible && colorCible.equals("V"))
								break;
						}
						else
							break;
					}
				}
			}

			else {
				if(robot.equals("R")){
					while(!cases[posXRed][posYRed].isDroite()){
						if(posXRed == cases[0].length-1)
							break;
						if(!cases[posXRed+1][posYRed].isGauche() && !(posXRed+1 == posXBlue && posYRed == posYBlue)
								&& !(posXRed+1 == posXYellow && posYRed == posYYellow) && !(posXRed+1 == posXGreen && posYRed == posYGreen)){
							deplX++;
							posXRed++;
							if(posXRed == posXCible && posYRed == posYCible && colorCible.equals("R"))
								break;
						}
						else
							break;
					}
				}
				else if(robot.equals("B")){
					while(!cases[posXBlue][posYBlue].isDroite()){
						if(posXBlue == cases[0].length-1)
							break;
						if(!cases[posXBlue+1][posYBlue].isGauche() && !(posXBlue+1 == posXRed && posYBlue == posYRed)
								&& !(posXBlue+1 == posXYellow && posYBlue == posYYellow) && !(posXBlue+1 == posXGreen && posYBlue == posYGreen)){
							deplX++;
							posXBlue++;
							if(posXBlue == posXCible && posYBlue == posYCible && colorCible.equals("B"))
								break;
						}
						else
							break;
					}
				}
				else if(robot.equals("J")){
					while(!cases[posXYellow][posYYellow].isDroite()){
						if(posXYellow == cases[0].length-1)
							break;
						if(!cases[posXYellow+1][posYYellow].isGauche() && !(posXYellow+1 == posXRed && posYYellow == posYRed)
								&& !(posXBlue == posXYellow+1 && posYBlue == posYYellow) && !(posXYellow+1 == posXGreen && posYYellow == posYGreen)){
							deplX++;
							posXYellow++;
							if(posXYellow == posXCible && posYYellow == posYCible && colorCible.equals("J"))
								break;
						}
						else
							break;
					}
				}
				else {
					while(!cases[posXGreen][posYGreen].isDroite()){
						if(posXGreen == cases[0].length-1)
							break;
						if(!cases[posXGreen+1][posYGreen].isGauche() && !(posXGreen+1 == posXRed && posYGreen == posYRed)
								&& !(posXGreen+1 == posXYellow && posYGreen == posYYellow) && !(posXBlue == posXGreen+1 && posYBlue == posYGreen)){
							deplX++;
							posXGreen++;
							if(posXGreen == posXCible && posYGreen == posYCible && colorCible.equals("V"))
								break;
						}
						else
							break;
					}
				}
			}

			if(direction.equals("H") || direction.equals("B"))
				tmp += robot + direction + deplY;
			else
				tmp += robot + direction + deplX;
			sol = sol.substring(2);
			deplX = 0;
			deplY = 0;
		}
		return tmp;
	}


}
