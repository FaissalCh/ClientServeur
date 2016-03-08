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
		Socket s;

		s = new Socket("localhost", port);
		in = new BufferedReader(new InputStreamReader(s.getInputStream()));
		out = new DataOutputStream(s.getOutputStream());

		String pseudo = "Omega";
		out.writeBytes("CONNEX/"+pseudo+"/\n");


		if(pseudo == "Omega2") {
			Thread.sleep(6000);
			out.writeBytes("TROUVE/moi/4\n");
		}
		
		while(true) {
			System.out.println(in.readLine());
		}
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
