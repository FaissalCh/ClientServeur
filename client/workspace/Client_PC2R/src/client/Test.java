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


		out.writeBytes("CONNEX/Omega/\n");
		//System.out.println(in.readLine()+" 1");
		//System.out.println(in.readLine()+" 2");
		//System.out.println(in.readLine()+" 3");
		//out.writeBytes("SORT/Omega3/\n");

		while(true)
			System.out.println(in.readLine()+" 1");
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
