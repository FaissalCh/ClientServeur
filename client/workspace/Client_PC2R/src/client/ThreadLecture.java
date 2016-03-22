package client;

import java.io.BufferedReader;
import java.io.IOException;

public class ThreadLecture extends Thread {

	private BufferedReader r;
	
	public ThreadLecture(BufferedReader r) {
		this.r = r;
	}
	
	@Override
	public void run() {
		String l;
		try {
			while((l = r.readLine()) != null) {
				System.out.println(l);
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
