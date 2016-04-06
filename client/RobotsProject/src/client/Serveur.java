package client;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.ServerSocket;
import java.net.Socket;

public class Serveur{

	public static void main(String[] args) throws IOException, InterruptedException {

		ServerSocket s =  new ServerSocket(2016);
		Socket c = s.accept();
		String userName;

		InputStream is = c.getInputStream();
		@SuppressWarnings("resource")
		InputStreamLiner isl = new InputStreamLiner(is);
		String line = isl.readLine(InputStreamLiner.UNIX);
		userName = line.split("/")[1];
		System.out.println("Le serveur recoit : " + line + " et de nom : " + userName);

		OutputStream os = c.getOutputStream();
		BufferedWriter br = new BufferedWriter(new OutputStreamWriter(os));

		/*if( !line.contains("CONNEXION/")){
			System.out.println("Serveur   ------>   PB");
			br.write("Error/" + "Login deja pris" + "/\n");
			br.flush();
		}
		else{*/
			br.write("BIENVENUE/" + userName + "/\n");
			br.flush();
		//}

		//Les murs
		br.write("SESSION/(3,4,H)(3,4,G)(12,6,H)(0,0,B)(14,12,H)(14,12,B)(15,14,D)/\n");
		br.flush();
		System.out.println("SESSION envoye!!");

		Thread.sleep(5000);

		//Les pions et la cible + bilan
		br.write("TOUR/(6,5,12,13,11,2,10,7,6,7,R)/1(saucisse, 0)(brouette, 0)(user2, 0)/\n");
		br.flush();
		System.out.println("TOUR envoye!!");

		//Thread.sleep(7000);

		//Send chatMessage
		/*br.write("CHAT/titi/Ca va et toi?/\n");
		br.flush();*/

		//Send chatMessage
		/*br.write("CHAT/toto/Une partie./\n");
		br.flush();*/

		Thread.sleep(2000);

		//Notification
		br.write("DECONNEXION/toto/\n");
		br.flush();
		System.out.println("USER 4 SENT");

		//Lit le nombre de coup propose
		line = isl.readLine(InputStreamLiner.UNIX);
		System.out.println("Le serveur pour NbCoup : " + line);

		//Il a trouve
		br.write("TUASTROUVE/\n");
		br.flush();

		/*br.write("FINREFLEXION/\n");
		br.flush();*/

		//Lit enchere 1
		line = isl.readLine(InputStreamLiner.UNIX);
		System.out.println("Le serveur pour Encher1 : " + line);

		//Envoi validation
		br.write("VALIDATION\n");
		br.flush();

		//Lit enchere 1
		line = isl.readLine(InputStreamLiner.UNIX);
		System.out.println("Le serveur pour Encher2 : " + line);

		//Envoi echec
		br.write("ECHEC/TOTO/\n");
		br.flush();

		Thread.sleep(500);

		br.write("FINENCHERE/m/8/\n");
		br.flush();

		//Recoit solution
		line = isl.readLine(InputStreamLiner.UNIX);
		System.out.println("Le serveur pour sol Finale : " + line);
		
		br.write("FINRESO/m\n");
		br.flush();
		
		Thread.sleep(2000);

		//Envoie la sol
		System.out.println("SEND SASOL ----->");
		br.write("SASOLUTION/m/VGRB/\n");
		br.flush();

		//Lit la sol finale
		br.write("BONNE/\n");
		br.flush();

		Thread.sleep(7000);

		//Lit la sol finale
		/*br.write("VAINQUEUR/2(saucisse, 1)(brouette, 0)/\n");
		br.flush();*/

		//Thread.sleep(10000);

		//Les murs
		/*br.write("SESSION/(7,8,H)(7,8,G)/\n");
		br.flush();
		System.out.println("SESSION2 envoye!!");*/

		Thread.sleep(2000);

		//Les pions et la cible + bilan
		br.write("TOUR/(3,5,12,13,11,2,10,8,6,7,V)/2(saucisse, 0)(brouette, 0)/\n");
		br.flush();
		System.out.println("TOUR2 envoye!!");




		c.close();
		s.close();

	}

}
