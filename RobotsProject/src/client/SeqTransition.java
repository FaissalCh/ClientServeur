package client;

import javafx.animation.FadeTransition;
import javafx.animation.SequentialTransition;
import javafx.animation.TranslateTransition;
import javafx.scene.image.ImageView;
import javafx.util.Duration;

public class SeqTransition {

	private ImageView iv;   //Image a deplacer
	private int a;         //Coordonnee selon les abscisses
	private int b;        //Coordonnee selon les ordonnees

	public SeqTransition(ImageView iv, int a, int b){
		this.iv = iv;
		this.a = a;
		this.b = b;
	}

	/*Enclenche une transition d'un robot donne sur une direction donnee*/
	public SequentialTransition seqTrans(double posx, double posy){

		int pos_a = Math.abs(a);
		int pos_b = Math.abs(b);

		
		FadeTransition ft = new FadeTransition();
		ft.setFromValue(1.0f);
		ft.setToValue(0.3f);
		ft.setCycleCount(2);
		ft.setAutoReverse(true);

		TranslateTransition tt = new TranslateTransition();
		tt.setFromX(iv.getX() + 42f*posx);
		tt.setToX(iv.getX() + 42f*posx + 42*a);
		tt.setCycleCount(1);
		tt.setAutoReverse(false);
		tt.setDuration(Duration.millis(500*pos_a));

		TranslateTransition tt2 = new TranslateTransition();
		tt2.setFromY(iv.getY() + 42f*posy);
		tt2.setToY(iv.getY() + 42f*posy + 42*b);
		tt2.setCycleCount(1);
		tt2.setAutoReverse(false);
		tt2.setDuration(Duration.millis(500*pos_b));

		return new SequentialTransition (iv, ft, tt, tt2);
	}

	public ImageView getIv() {
		return iv;
	}

}
