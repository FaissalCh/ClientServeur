package client;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.net.UnknownHostException;

public class Test {

	public static void main(String[] args) throws UnknownHostException, IOException, InterruptedException {
		int port = 2016;
		BufferedReader in;
		DataOutputStream out;
		Thread t;
		Socket s;

		s = new Socket("localhost", port);
		in = new BufferedReader(new InputStreamReader(s.getInputStream()));
		out = new DataOutputStream(s.getOutputStream());
		t = new ThreadLecture(in);

		String pseudo = "Omega1";
		out.writeBytes("CONNEXION/"+pseudo+"/\n");

		// TROUVE/Omega1/5
		// ENCHERE/Omega1/4
		BufferedReader cons = new BufferedReader(new InputStreamReader(System.in));
		String l;
		t.start();
		while((l = cons.readLine()) != null) {
			out.writeBytes(l+"\n");
		}
		//		if(pseudo == "Omega2") {
		//			Thread.sleep(6000);
		//			out.writeBytes("TROUVE/moi/4\n");
		//		}
		//
		//		while(true) {
		//			System.out.println(in.readLine());
		//		}
		//		System.out.println("Fermeture client");
		//
		//		
		//
		//
		//
		//		in.close();
		//		out.close();
		//		s.close();
	}

}