package client;
import java.io.IOException;
import java.io.InputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import javafx.animation.PauseTransition;
import javafx.animation.SequentialTransition;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.concurrent.Task;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.PasswordField;
import javafx.scene.control.ProgressBar;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.scene.effect.DropShadow;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.shape.Line;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;
import javafx.scene.text.Text;
import javafx.stage.Popup;
import javafx.stage.Stage;
import javafx.util.Duration;

public class Client extends Application{

	Socket c;
	InetSocketAddress localhost;
	String userName, userActif, pw, session;
	Stage stagePrinc = new Stage();
	String bilan;
	Button buttonDisconnect, buttonSubmit;

	boolean animation = false;
	int index = 0;
	int score = 0;
	int nbPlayers = 1;
	double enchereCourante = Double.POSITIVE_INFINITY;
	double enchereTappe = Double.POSITIVE_INFINITY;
	String solution;
	EtatCase cases[][];
	Couleur color;               //Couleur de la cible
	ArrayList<Integer> liste;    //Pour la position des pions
	HashMap<ImageView, ArrayList<Integer>> map = new HashMap<>();

	ImageView redPawn_img = new ImageView();
	ImageView bluePawn_img = new ImageView();
	ImageView yellowPawn_img = new ImageView();
	ImageView greenPawn_img = new ImageView();
	ImageView cible_img = new ImageView();

	VBox layout = new VBox(60);
	GridPane grid = new GridPane();   //Plateau avec cases + murs
	BorderPane bp = new BorderPane();  //Plateau + bouttons
	HBox hb = new HBox(10);
	Label labelNotify = new Label();
	Label labelSolution = new Label("Enter your solution :");
	TextField textSolution = new TextField();
	Label labelBilan = new Label("");
	Label labelScore = new Label("Score : " + score);
	Label labelNbMoves = new Label("Enter the number of moves :");
	Label labelEnchere = new Label("Make a bid :");
	TextField textEnchere = new TextField();
	Label labelNbJoueur = new Label("[Number of players] : " + nbPlayers);
	Label labelTimer = new Label();
	Label labelEnchereCourante = new Label();
	int tempsReflexion, tempsEnchere, tempsResolution;

	Timer timerReflexion, timerEnchere, timerResolution;

	VBox infoBox = new VBox(10);
	TextArea chatBox = new TextArea();
	TextField chatText = new TextField();
	VBox chat = new VBox(15);

	final Popup popup = new Popup(); 
	Text textPopup = new Text();

	DateFormat dateFormat = new SimpleDateFormat("HH:mm:ss");


	public static void main(String[] args) {
		launch(args);
	}


	@Override
	public void start(Stage stage) throws Exception{


		Button buttonConnect, buttonPrivateParty, sendMessageButton, buttonCreateSession;

		labelNotify.setFont(new Font("Arial", 18));
		labelNotify.setTextFill(Color.web("#6600ff"));
		labelTimer.setFont(new Font("Arial", 14));
		labelTimer.setTextFill(Color.web("#ff0000"));
		labelEnchereCourante.setFont(new Font("Arial", 14));
		labelEnchereCourante.setTextFill(Color.web("#0073e6"));
		labelNbJoueur.setFont(new Font("Arial", 14));
		labelNbJoueur.setTextFill(Color.web("#00cc66"));
		labelScore.setFont(new Font("Arial", 14));
		labelScore.setTextFill(Color.web("#00cc66"));
		Label labelPseudo = new Label();
		labelPseudo.setFont(new Font("Arial", 14));
		labelPseudo.setTextFill(Color.web("#00cc66"));
		labelPseudo.getStyleClass().add("rounded-label");
		TextField textFieldName = new TextField();

		labelNbMoves.setFont(Font.font("Verdana", FontWeight.BOLD, 14));
		labelEnchere.setFont(Font.font("Verdana", FontWeight.BOLD, 14));
		labelSolution.setFont(Font.font("Verdana", FontWeight.BOLD, 14));
		labelNbMoves.setTranslateY(4);
		labelEnchere.setTranslateY(4);
		labelSolution.setTranslateY(4);


		/************On se connecte************/
		stage.setTitle("ConnexionWindow");
		buttonConnect = new Button();
		buttonConnect.setText("Join A Game");
		buttonPrivateParty = new Button();
		buttonPrivateParty.setText("Join Private Game");
		buttonCreateSession = new Button();
		buttonCreateSession.setText("Create Session");
		sendMessageButton = new Button();
		sendMessageButton.setText("Send A Message");
		Stage stageConnexion = new Stage();

		Label labelAdIP = new Label("@ IP : ");
		labelAdIP.setFont(Font.font("Verdana", FontWeight.BOLD, 12));
		TextField textAdIP = new TextField();
		textAdIP.setText("127.0.0.1");
		Label labelErrorIP = new Label();
		labelErrorIP.setTextFill(Color.web("#ff0000"));

		Label labelName = new Label("Your name : ");
		Label labelErrorName = new Label();

		Label labelNamePrivate = new Label("Login : ");
		TextField textFieldLogin = new TextField();
		Label labelError = new Label();
		labelError.setTranslateX(20);
		labelError.setTextFill(Color.web("#ff0000"));
		Label labelErrorCreate = new Label();
		labelErrorCreate.setTranslateX(20);
		labelErrorCreate.setTextFill(Color.web("#ff0000"));
		Label labelPasswordPrivate = new Label("Password :");
		TextField textFieldPassword = new PasswordField();
		Label labelErrorEntering = new Label();
		labelErrorEntering.setTextFill(Color.web("#ff0000"));

		Label labelSession = new Label("Name of the session : ");
		TextField textFieldSession = new TextField();
		Label labelcreateSession = new Label("Session to create : ");
		TextField textFieldCreateSession = new TextField();
		Label labelLoginSession = new Label("Login : ");
		TextField textFieldLoginSession = new TextField();
		Label labelPwSession = new Label("Password : ");
		TextField textFieldPwSession = new PasswordField();
		Label labelNomSession = new Label();

		HBox layoutAdIP = new HBox(5);
		layoutAdIP.getChildren().addAll(labelAdIP, textAdIP);
		VBox layoutAdIPBis = new VBox(10);
		layoutAdIPBis.getChildren().addAll(layoutAdIP, labelErrorIP);
		layoutAdIPBis.setPadding(new Insets(45, 0, 0, 20));

		HBox layoutCreateSession1 = new HBox(5);
		layoutCreateSession1.getChildren().addAll(labelcreateSession, textFieldCreateSession);
		layoutCreateSession1.setPadding(new Insets(0, 0, 0, 39));
		HBox layoutCreateSession2 = new HBox(5);
		layoutCreateSession2.getChildren().addAll(labelLoginSession, textFieldLoginSession);
		layoutCreateSession2.setPadding(new Insets(0, 0, 0, 117));
		HBox layoutCreateSession3 = new HBox(5);
		layoutCreateSession3.getChildren().addAll(labelPwSession, textFieldPwSession, buttonCreateSession);
		layoutCreateSession3.setPadding(new Insets(0, 0, 0, 90));

		HBox layoutEnterSession = new HBox(5);
		layoutEnterSession.getChildren().addAll(labelSession, textFieldSession);
		layoutEnterSession.setPadding(new Insets(0, 0, 0, 15));

		HBox layoutLogin = new HBox(5);
		layoutLogin.getChildren().addAll(labelNamePrivate, textFieldLogin);
		layoutLogin.setPadding(new Insets(0, 0, 0, 111));

		HBox layoutPassword = new HBox(5);
		layoutPassword.getChildren().addAll(labelPasswordPrivate, textFieldPassword, buttonPrivateParty);
		layoutPassword.setPadding(new Insets(0, 0, 0, 89));

		HBox layoutName = new HBox(5);
		layoutName.getChildren().addAll(labelName, textFieldName, buttonConnect);
		layoutName.setPadding(new Insets(50, 0, 0, 84));
		VBox layoutNameBis = new VBox(10);
		layoutNameBis.getChildren().addAll(layoutName, labelErrorName);
		labelErrorName.setTranslateX(20);
		labelErrorName.setTextFill(Color.web("#ff0000"));

		VBox box1 = new VBox(10);
		box1.getChildren().addAll(layoutCreateSession1, layoutCreateSession3, layoutCreateSession2, labelErrorCreate);

		VBox box2 = new VBox(10);
		box2.getChildren().addAll(layoutEnterSession, layoutPassword, layoutLogin, labelError);

		Line line = new Line();
		line.setStartX(0.0f);
		line.setStartY(0.0f);
		line.setEndX(600.0f);
		line.setStroke(Color.web("#99bbff"));

		Line line2 = new Line();
		line2.setStartX(0.0f);
		line2.setStartY(0.0f);
		line2.setEndX(800.0f);
		line2.setStroke(Color.web("#99bbff"));

		Line line3 = new Line();
		line3.setStartX(0.0f);
		line3.setStartY(0.0f);
		line3.setEndX(1000.0f);
		line3.setStroke(Color.web("#99bbff"));

		VBox connexionBox = new VBox(40);
		connexionBox.getChildren().addAll(layoutNameBis, line, box1, line2, box2, line3, layoutAdIPBis);
		layoutAdIPBis.setTranslateY(-40);


		popup.setX(600); popup.setY(250);
		textPopup.setFont(Font.font ("Verdana", 40));
		textPopup.setFill(Color.web("#002db3"));
		popup.getContent().addAll(textPopup);



		/*Pour que le pseudo ne contienne pas de slash*/
		textFieldName.textProperty().addListener((ov, oldValue, newValue) -> {
			if(newValue.contains("/"))
				textFieldName.setText(oldValue);
		});

		/*Pour que le pseudo ne contienne pas de slash*/
		textFieldLogin.textProperty().addListener((ov, oldValue, newValue) -> {
			if(newValue.contains("/"))
				textFieldLogin.setText(oldValue);
		});

		/*Pour que le pseudo ne contienne pas de slash*/ 
		textFieldLoginSession.textProperty().addListener((ov, oldValue, newValue) -> {
			if(newValue.contains("/"))
				textFieldLoginSession.setText(oldValue);
		});

		/*Pour que la session ne contienne pas de slash*/
		textFieldSession.textProperty().addListener((ov, oldValue, newValue) -> {
			if(newValue.contains("/"))
				textFieldSession.setText(oldValue);
		});

		/*Pour que la session ne contienne pas de slash*/ 
		textFieldCreateSession.textProperty().addListener((ov, oldValue, newValue) -> {
			if(newValue.contains("/"))
				textFieldCreateSession.setText(oldValue);
		});

		/*Pour que le password ne contienne pas de slash*/
		textFieldPassword.textProperty().addListener((ov, oldValue, newValue) -> {
			if(newValue.contains("/"))
				textFieldPassword.setText(oldValue);
		});

		/*Pour que le password ne contienne pas de slash*/ 
		textFieldPwSession.textProperty().addListener((ov, oldValue, newValue) -> {
			if(newValue.contains("/"))
				textFieldPwSession.setText(oldValue);
		});



		buttonConnect.setTranslateX(20);
		buttonConnect.setTranslateY(-3);
		buttonConnect.setStyle("-fx-font: 18 arial; -fx-base: #b6e7c9;");
		buttonConnect.setOnAction(e -> {
			boolean prisNom = false, valideAdresseIP = false;
			if ((textFieldName.getText() != null && !textFieldName.getText().isEmpty())) {
				userName = textFieldName.getText();
				prisNom = true;
				labelErrorName.setText("");
			} else {
				labelErrorName.setText("You have to choose a name");
			}
			if(prisNom){
				if((textAdIP.getText() != null && !textAdIP.getText().isEmpty())){
					c = new Socket();
					localhost = new InetSocketAddress(textAdIP.getText(), 2016);
					try {
						c.connect(localhost);
						valideAdresseIP = true;
					} catch (Exception e1) {
						labelErrorIP.setText("Wrong IP adress");
					}
				}
				else{
					labelErrorIP.setText("Choose an IP@");
					textAdIP.setText("");
				}
			}
			if(prisNom && valideAdresseIP){
				String retour = null;
				try {
					retour = Tools.signalerConnexion(c, userName);  //Le client signal son arrive et attend le signal BIENVENU
				} catch (Exception e2) {}    
				if(retour.startsWith("BIENVENUE/")){
					System.out.println("The Party begin now");
					labelPseudo.setText("[Pseudo] : "+ userName); 
					labelNomSession.setText("Session Public");
					stageConnexion.close();
				} else
					try {
						labelErrorName.setText(retour.split("/")[1]);
						textFieldName.setText("");
					} catch (Exception e1) {}
			}
		});



		buttonPrivateParty.setTranslateY(-20);
		buttonPrivateParty.setTranslateX(20);
		buttonPrivateParty.setStyle("-fx-font: 18 arial; -fx-base: #b6e7c9;");
		buttonPrivateParty.setOnAction(e -> {
			boolean prisChamps = false, valideAdresseIP = false;
			if ((textFieldLogin.getText() != null && !textFieldLogin.getText().isEmpty()) && (textFieldPassword.getText() != null && 
					!textFieldPassword.getText().isEmpty()) && (textFieldSession.getText() != null && !textFieldSession.getText().isEmpty())
					&& (textAdIP.getText() != null && !textAdIP.getText().isEmpty())) {
				userName = textFieldLogin.getText();
				pw = textFieldPassword.getText();
				session = textFieldSession.getText();
				prisChamps = true;
				labelError.setText("");
			} else {
				labelError.setText("You have to choose a session, a login, and a password");
			}
			if(prisChamps){
				if((textAdIP.getText() != null && !textAdIP.getText().isEmpty())){
					c = new Socket();
					localhost = new InetSocketAddress(textAdIP.getText(), 2016);
					try {
						c.connect(localhost);
						valideAdresseIP = true;
					} catch (Exception e1) {
						labelErrorIP.setText("Wrong IP adress");
						textAdIP.setText("");
					}
				}
				else{
					labelErrorIP.setText("Choose an IP@");
					textAdIP.setText("");
				}
			}
			if(prisChamps && valideAdresseIP){
				String retour = null;
				try {
					retour = Tools.signalerConnexionPrivee(c, session, userName, pw, false); //Le client signal son arrive et attend le signal BIENVENU
				} catch (Exception e2) {}    
				if(retour.startsWith("BIENVENUE/")){
					System.out.println("The Party begin now");
					labelPseudo.setText("Pseudo : ["+ userName + "]");
					labelNomSession.setText("Session " + session);
					stageConnexion.close();
				} else
					try {
						labelError.setText(retour.split("/")[1]);
						textFieldLogin.setText("");
						textFieldPassword.setText("");
						textFieldSession.setText("");
					} catch (Exception e1) {}
			}
		});


		buttonCreateSession.setTranslateY(-20);
		buttonCreateSession.setTranslateX(20);
		buttonCreateSession.setStyle("-fx-font: 18 arial; -fx-base: #b6e7c9;");
		buttonCreateSession.setOnAction(e -> {
			boolean prisChamps = false, valideAdresseIP = false;
			if ((textFieldCreateSession.getText() != null && !textFieldCreateSession.getText().isEmpty()) && (textFieldLoginSession.getText() != null && 
					!textFieldLoginSession.getText().isEmpty()) && (textFieldPwSession.getText() != null && !textFieldPwSession.getText().isEmpty())
					&& (textAdIP.getText() != null && !textAdIP.getText().isEmpty())) {
				userName = textFieldLoginSession.getText();
				pw = textFieldPwSession.getText();
				session = textFieldCreateSession.getText();
				prisChamps = true;
				labelErrorCreate.setText("");
			} else {
				labelErrorCreate.setText("You have to choose a session, a login, and a password");
			}
			if(prisChamps){
				if((textAdIP.getText() != null && !textAdIP.getText().isEmpty())){
					c = new Socket();
					localhost = new InetSocketAddress(textAdIP.getText(), 2016);
					try {
						c.connect(localhost);
						valideAdresseIP = true;
					} catch (Exception e1) {
						labelErrorIP.setText("Wrong IP adress");
						textAdIP.setText("");
					}
				}
				else{
					labelErrorIP.setText("Choose an IP@");
					textAdIP.setText("");
				}
			}
			if(prisChamps && valideAdresseIP){
				String retour = null;
				try {
					retour = Tools.signalerConnexionPrivee(c, session, userName, pw, true);
				} catch (Exception e2) {}    
				if(retour.startsWith("BIENVENUE/")){   //Le client signal son arrive et attend le signal BIENVENU
					System.out.println("The Party begin now");
					labelPseudo.setText("Pseudo : ["+ userName + "]");
					labelNomSession.setText("Session " + session);
					stageConnexion.close();
				} else
					try {
						labelErrorCreate.setText(retour.split("/")[1]);
						textFieldCreateSession.setText("");
						textFieldLoginSession.setText("");
						textFieldPwSession.setText("");
					} catch (Exception e1) {}
			}
		});


		sendMessageButton.setStyle("-fx-font: 18 arial; -fx-base: #00cccc;");
		sendMessageButton.setTranslateX(60);
		sendMessageButton.setOnAction(e -> {
			Date date = new Date();
			try {
				Tools.sendMessage(c, userName, chatText.getText());
			} catch (Exception e1) {}
			chatBox.appendText("[" + dateFormat.format(date) + "]" + "<" + userName + "> : " + chatText.getText() + "\n");
			chatText.setText("");
		});


		stageConnexion.setTitle("Connexion Window");
		stageConnexion.setResizable(false);
		Scene sceneConnexion = new Scene(connexionBox, 600, 700);

		/*Obligatoire pour display stage2*/
		stage.setScene(sceneConnexion);

		stageConnexion.setScene(sceneConnexion);
		stageConnexion.setOnCloseRequest(e -> {  //Si on ferme la fenetre tout s arrete
			try {
				stageConnexion.close();
				c.close();
				System.exit(0);
			} catch (Exception e1) {}
			System.out.println("U're disconnected");
		});

		stageConnexion.showAndWait();


		buttonDisconnect = new Button();
		buttonDisconnect.setMinSize(25, 35);
		buttonDisconnect.setStyle("-fx-font: 22 arial; -fx-base: #e6005c;");
		buttonDisconnect.setText("Quit Party");
		buttonDisconnect.addEventHandler(MouseEvent.MOUSE_ENTERED, 
				new EventHandler<MouseEvent>() {
			@Override public void handle(MouseEvent e) {
				buttonDisconnect.setEffect(new DropShadow());
			}
		});
		buttonDisconnect.addEventHandler(MouseEvent.MOUSE_EXITED, 
				new EventHandler<MouseEvent>() {
			@Override public void handle(MouseEvent e) {
				buttonDisconnect.setEffect(null);
			}
		});
		buttonDisconnect.setOnAction(e -> {
			try {
				Tools.deconnexion(c, userName);
				stagePrinc.close();
				c.close();
				System.exit(0);
			} catch (Exception e1) {}
			System.out.println("U're disconnected");
		});


		/*******Agencement de l'interface*******/
		buttonSubmit = new Button();
		buttonSubmit.setText("Submit");

		labelBilan.setTextFill(Color.web("#1a53ff"));
		labelBilan.setFont(new Font("Arial", 15));
		labelBilan.setTranslateY(20);


		Label labelCalculNbCoup = new Label("Test : ");
		Label labelNbCoups = new Label("0");
		TextField textCalculNbCoups = new TextField();
		HBox boxCalculNbCoups = new HBox(10);
		boxCalculNbCoups.getChildren().addAll(labelCalculNbCoup, textCalculNbCoups, labelNbCoups);


		labelCalculNbCoup.setFont(Font.font("Verdana", FontWeight.BOLD, 16));
		labelCalculNbCoup.setTextFill(Color.web("#6b6b47"));
		labelCalculNbCoup.setFont(Font.font("Verdana", FontWeight.BOLD, 15));
		labelCalculNbCoup.setTextFill(Color.web("#6b6b47"));
		labelCalculNbCoup.setTranslateY(4);
		labelNbCoups.setTranslateY(5);

		textCalculNbCoups.textProperty().addListener((ov, oldValue, newValue) -> {
			if(!newValue.isEmpty()) {
				newValue = newValue.toUpperCase();
				char c = newValue.charAt(newValue.length()-1);
				if(c != 'R' && c != 'B' && c != 'J' && c != 'V' && c != 'H' && c != 'G' && c != 'D'){
					textCalculNbCoups.setText(oldValue.toUpperCase());
				}
				else
					textCalculNbCoups.setText(newValue.toUpperCase());
				labelNbCoups.setText(String.valueOf(newValue.length()/2));
			}
		});

		VBox hbNotify = new VBox(30);
		hbNotify.getChildren().addAll(labelPseudo, labelScore, labelNbJoueur);
		hbNotify.setTranslateX(20);
		HBox disconnectBox = new HBox(20);
		disconnectBox.getChildren().addAll(buttonDisconnect,labelErrorEntering);
		disconnectBox.setPadding(new Insets(0, 0, 0, 80));
		HBox boxTimerEnchere = new HBox(30);
		boxTimerEnchere.getChildren().addAll(labelTimer, labelEnchereCourante);

		layout.getChildren().addAll(disconnectBox, boxCalculNbCoups, hb, hbNotify, boxTimerEnchere, labelNotify, labelBilan);
		layout.setPadding(new Insets(10, 0, 0, 5));


		grid.setPadding(new Insets(10, 10, 10, 10));
		grid.setVgap(2);
		grid.setHgap(2);

		Scene scene = new Scene(bp, 1500, 700);

		/*Pour que la solution soit en lettre majuscule*/
		textSolution.textProperty().addListener((ov, oldValue, newValue) -> {
			textSolution.setText(newValue.toUpperCase());
		});


		buttonSubmit.setMaxSize(65, 25);
		buttonSubmit.setStyle("-fx-font: 15 arial; -fx-base: #ffff4d;");
		buttonSubmit.setText("Send");
		buttonSubmit.setOnAction(e -> {
			try {
				if(hb.getChildren().contains(labelNbMoves)){
					if(Tools.isNumericAndNotZero(textSolution.getText())){
						enchereCourante = Double.valueOf(textSolution.getText());
						Tools.sendNbMoves(c, userName, textSolution.getText());
						labelErrorEntering.setText("");
					}
					else{
						labelErrorEntering.setText("Please enter a digital, positive value");
					}
					textSolution.setText("");
				}

				else if(hb.getChildren().contains(labelEnchere)){
					if(Tools.isNumericAndNotZero(textEnchere.getText())){
						enchereCourante = Double.valueOf(textEnchere.getText());
						Tools.sendEnchere(c, userName, textEnchere.getText());
						labelErrorEntering.setText("");
					}
					else{
						labelErrorEntering.setText("Please enter a digital, positive value");
					}
					textEnchere.setText("");
				}

				else{
					textEnchere.setText("");
					if(Tools.isCorrectMove(textSolution.getText())){
						Tools.sendSolution(c, userName, textSolution.getText());
						hb.getChildren().removeAll(hb.getChildren());
						if(timerResolution != null)
							timerResolution.cancel();
						labelTimer.setText("");
						labelErrorEntering.setText("");
					}
					else{
						labelErrorEntering.setText("Please enter a correct serie of moves");
					}
					textSolution.setText("");
				}
			}catch(Exception e1){}
		});



		stagePrinc.setTitle("Enigme : " + labelNomSession.getText());
		stagePrinc.setScene(scene);



		@SuppressWarnings("rawtypes")
		Task task = new Task<Void>() {
			@Override public Void call() throws IOException {
				process();
				return null;
			}
		};

		ProgressBar bar = new ProgressBar();
		bar.progressProperty().bind(task.progressProperty());
		new Thread(task).start();



		chatBox.setMaxSize(300, 700);
		chatBox.setMinSize(300, 600);
		chatBox.setPadding(new Insets(10, 0, 0, 0));
		chatBox.setDisable(false);
		chatBox.setEditable(false);
		chatBox.setFocusTraversable(false);

		chatText.setPromptText("Enter your message ...");
		chat.getChildren().addAll(chatBox, chatText, sendMessageButton);


		stagePrinc.setResizable(false);  //Pour afficher correctement sur toute taille d'ecran
		stagePrinc.setOnCloseRequest(e -> {  //Si on ferme la fenetre tout s arrete
			try {
				Tools.deconnexion(c, userName);
				stagePrinc.close();
				c.close();
				System.exit(0);
			} catch (Exception e1) {}
			System.out.println("U're disconnected");
		});

		stagePrinc.showAndWait();

	}



	/*************Methode a lancer pour MAJ de l'interface*****************/
	public void process() throws IOException{


		while(!buttonDisconnect.isPressed()){    //On ne s arrete pas tant que lon ne s est pas deconnecte

			InputStream is = c.getInputStream();
			@SuppressWarnings("resource")
			InputStreamLiner isl = new InputStreamLiner(is);
			String line = isl.readLine(InputStreamLiner.UNIX);

			System.out.println("user : " +userName + " " + line); 

			if(line.startsWith("SESSION/")){    //Debut SESSION

				Platform.runLater(new Runnable(){
					@Override public void run() {

						textPopup.setText("Début d'une nouvelle partie");
						popup.show(stagePrinc);
						PauseTransition delay = new PauseTransition(Duration.seconds(3));
						delay.setOnFinished( event -> textPopup.setText("") );
						delay.play();

						cases = new EtatCase[16][16];
						try {
							cases = Tools.getPlateau(line.split("/")[1]);
						} catch (NumberFormatException e2) {}
						catch (IOException e2) {}  //Construit le plateau


						Image image = new Image("murs/case.png");

						List<ImageView> briks = new ArrayList<ImageView>();
						boolean haut = false, bas = false, gauche = false, droite = false;
						boolean case_vide = false;

						/************Put the walls***********/
						for(int i = 0; i < 16; i++){
							for(int j = 0; j < 16; j++){
								ImageView iv1 = new ImageView();
								String type_mur = "murs/";

								if(i == 0 || j == 0 || i == 15 || j == 15){

									if(j == 0)
										haut = true;

									if(j == 15)
										bas = true;

									if(i == 0)
										gauche = true;

									if(i == 15)
										droite = true;

								}

								if(cases[i][j].isHaut() || cases[i][j].isBas() || cases[i][j].isGauche() || cases[i][j].isDroite()){
									if(cases[i][j].isHaut()){
										haut = true;
									}
									if(cases[i][j].isBas()){
										bas = true;
									}
									if(cases[i][j].isGauche()){
										gauche = true;
									}
									if(cases[i][j].isDroite()){
										droite = true;
									}
								}

								else if(i != 0 && j != 0 && i != 15 && j != 15){
									iv1.setImage(image);
									case_vide = true;
								}

								if(!case_vide){
									type_mur = "murs/";

									if(haut)
										type_mur += "haut";

									if(bas)
										type_mur += "-bas";

									if(gauche)
										type_mur += "-gauche";

									if(droite)
										type_mur += "-droite";

									if(type_mur.equals("murs/haut-bas-gauche-droite"))
										type_mur = "murs/plein";

									iv1.setImage(new Image(type_mur+".png"));
								}


								GridPane.setConstraints(iv1, i, j);
								briks.add(iv1);
								haut = false; bas = false; gauche = false; droite = false;
								case_vide = false;
							}
						}


						labelScore.setText("[Score] : " + score);
						grid.getChildren().removeAll(grid.getChildren());
						grid.getChildren().addAll(briks);


						HBox top = new HBox(5);
						top.getChildren().addAll(grid, layout, chat);

						bp.setTop(top);
						//bp.setCenter(layout);
					}
				});


			}   //Fin SESSION


			/**********Affichage de la position des robots et de la cible**********/
			else if(line.startsWith("TOUR/")){          //Debut TOUR

				Platform.runLater(new Runnable(){
					@Override public void run() {

						/*On enleve les anciennes positions des robots et la cible*/
						if(grid.getChildren().contains(redPawn_img))
							grid.getChildren().remove(redPawn_img);
						if(grid.getChildren().contains(bluePawn_img))
							grid.getChildren().remove(bluePawn_img);
						if(grid.getChildren().contains(yellowPawn_img))
							grid.getChildren().remove(yellowPawn_img);
						if(grid.getChildren().contains(greenPawn_img))
							grid.getChildren().remove(greenPawn_img);

						if(grid.getChildren().contains(cible_img))
							grid.getChildren().remove(cible_img);


						String linePos = (line.split("/")[1]).substring(1, line.split("/")[1].length()-1);
						PosRobots positionRobots = null;
						try {
							positionRobots = Tools.getRobots(linePos);
						} catch (IOException e2) {}

						liste = positionRobots.getPositions();
						color = positionRobots.getColor();

						List<ImageView> briks2 = new ArrayList<ImageView>();

						int i = 0;
						while(i < 9){
							int a = liste.get(i);
							int b = liste.get(i+1);
							ImageView iv1 = new ImageView();
							if(i == 0){
								iv1.setImage(new Image("pawns/redPawn.png"));
								redPawn_img = iv1;
							}
							else if(i == 2){
								iv1.setImage(new Image("pawns/bluePawn.png"));
								bluePawn_img = iv1;
							}
							else if(i == 4){
								iv1.setImage(new Image("pawns/yellowPawn.png"));
								yellowPawn_img = iv1;
							}
							else if(i == 6){
								iv1.setImage(new Image("pawns/greenPawn.png"));
								greenPawn_img = iv1;
							}
							else{
								if(color == Couleur.R)
									iv1.setImage(new Image("circle/redCircle.png"));
								else if(color == Couleur.B)
									iv1.setImage(new Image("circle/blueCircle.png"));
								else if(color == Couleur.J)
									iv1.setImage(new Image("circle/yellowCircle.png"));
								else
									iv1.setImage(new Image("circle/greenCircle.png"));
								cible_img = iv1;
							}
							GridPane.setConstraints(iv1, a, b);
							GridPane.setMargin(iv1, new Insets(0, 0, 0, 5));
							i += 2;
						}

						briks2.add(cible_img);   //Dans cet ordre pour que les robots passe sur la cible et pas en-dessous
						briks2.add(redPawn_img);
						briks2.add(bluePawn_img);
						briks2.add(yellowPawn_img);
						briks2.add(greenPawn_img);

						grid.getChildren().addAll(briks2);

						HBox top = new HBox(5);
						top.getChildren().addAll(grid, layout, chat);
						bp.setTop(top);


						bilan = line.split("/")[2];
						bilan = "[Tour] = " + bilan.charAt(0) + "\n[Scores] = " + bilan.substring(1, bilan.length());

						labelBilan.setText(bilan);
						nbPlayers= bilan.split("\\(").length - 1;

						labelNbJoueur.setText("[Number of players] : " + nbPlayers);
						hb.getChildren().removeAll(hb.getChildren());
						hb.getChildren().addAll(labelNbMoves, textSolution, buttonSubmit);

						tempsReflexion = 300;   //En secondes   ---> 5 min
						/*Demarage du timer pour la phase de reflexion*/
						timerReflexion = new Timer();
						timerReflexion.schedule(
								new TimerTask() {

									@Override
									public void run() {
										Platform.runLater(new Runnable(){
											@Override public void run() {
												labelTimer.setText("[Temps restant] = " + (tempsReflexion/60) + " min, " + (tempsReflexion%60) + " sec");
												if(tempsReflexion > 0)
													tempsReflexion--;
											}
										});
									}
								}, 0, 1000);

						popup.show(stagePrinc);
						textPopup.setText("Phase de reflexion");
						PauseTransition delay = new PauseTransition(Duration.seconds(3));
						delay.setOnFinished( event -> textPopup.setText("") );
						delay.play();
					}
				});

			}          //Fin TOUR

			else if(line.startsWith("TUASTROUVE/") || line.startsWith("ILATROUVE/") || line.startsWith("FINREFLEXION/")){

				tempsEnchere = 30;
				/*Demarage du timer pour l'enchere*/
				timerEnchere = new Timer();
				timerEnchere.schedule(
						new TimerTask() {

							@Override
							public void run() {
								Platform.runLater(new Runnable(){
									@Override public void run() {
										labelTimer.setText("[Temps restant] = " + (tempsEnchere/60) + " min, " + (tempsEnchere%60) + " sec");
										if(tempsEnchere > 0)
											tempsEnchere--;
									}
								});
							}
						}, 0, 1000);

				Platform.runLater(new Runnable(){
					@Override public void run() {
						popup.show(stagePrinc);
						textPopup.setText("Phase d'enchere");
						PauseTransition delay = new PauseTransition(Duration.seconds(3));
						delay.setOnFinished( event -> textPopup.setText("") );
						delay.play();
					}
				});

				if(line.startsWith("TUASTROUVE/")){
					Platform.runLater(new Runnable(){
						@Override public void run() {
							if(timerReflexion != null)
								timerReflexion.cancel();
							labelTimer.setText("");
							labelNotify.setText("Je pense avoir trouvé");
							hb.getChildren().removeAll(hb.getChildren());
							hb.getChildren().addAll(labelEnchere, textEnchere, buttonSubmit);
							labelEnchereCourante.setText("[Derniere enchere] = " + (int)enchereCourante);
						}
					});
				}

				else if(line.startsWith("ILATROUVE/")){
					Platform.runLater(new Runnable(){
						@Override public void run() {
							if(timerReflexion != null)
								timerReflexion.cancel();
							labelTimer.setText("");
							labelNotify.setText("[" + line.split("/")[1] + "] a une solution en " + line.split("/")[2] + " coups");
							hb.getChildren().removeAll(hb.getChildren());
							hb.getChildren().addAll(labelEnchere, textEnchere, buttonSubmit);
						}
					});
				}

				else{      //Ici FINREFLEXION
					Platform.runLater(new Runnable(){
						@Override public void run() {
							if(timerReflexion != null)
								timerReflexion.cancel();
							labelTimer.setText("");
							labelNotify.setText("FIN DU TEMPS IMPARTI ET DE LA PHASE DE REFLEXION");
							hb.getChildren().removeAll(hb.getChildren());
							hb.getChildren().addAll(labelEnchere, textEnchere, buttonSubmit);
						}
					});
				}
			}


			else if(line.startsWith("FINENCHERE/")){

				Platform.runLater(new Runnable(){
					@Override public void run() {
						if(timerEnchere != null)
							timerEnchere.cancel();
						labelTimer.setText("");
						userActif = line.split("/")[1];
						String nbCoups = line.split("/")[2];
						labelNotify.setText("Fin des encheres, le joueur actif est\n [" + userActif + "] pour " + nbCoups + " coups");
						hb.getChildren().removeAll(hb.getChildren());
						if(userName.equals(userActif)){
							hb.getChildren().addAll(labelSolution, textSolution, buttonSubmit);

							tempsResolution = 60;
							/*Demarage du timer pour la phase de resolution*/
							timerResolution = new Timer();
							timerResolution.schedule(
									new TimerTask() {

										@Override
										public void run() {
											Platform.runLater(new Runnable(){
												@Override public void run() {
													labelTimer.setText("[Temps restant] = " + (tempsResolution/60) + " min, " + (tempsResolution%60) + " sec");
													if(tempsResolution > 0)
														tempsResolution--;
												}
											});
										}
									}, 0, 1000);
						}

						popup.show(stagePrinc);
						textPopup.setText("Phase de résolution");
						PauseTransition delay = new PauseTransition(Duration.seconds(3));
						delay.setOnFinished( event -> textPopup.setText("") );
						delay.play();

						labelEnchereCourante.setText("");
					}
				});

			}


			else if(line.startsWith("VALIDATION")){
				Platform.runLater(new Runnable(){
					@Override public void run() {
						labelNotify.setText("Validation de mon enchere");
						labelEnchereCourante.setText("[Derniere enchere] = " + (int)enchereCourante);
					}
				});
			}

			else if(line.startsWith("ECHEC/")){
				Platform.runLater(new Runnable(){
					@Override public void run() {
						labelNotify.setText("Echec de mon enchere car incoherente avec : " + line.split("/")[1]);
					}
				});
			}

			else if(line.startsWith("NOUVELLEENCHERE/")){
				Platform.runLater(new Runnable(){
					@Override public void run() {
						labelNotify.setText("[" + line.split("/")[1] + "] a encheri de " + line.split("/")[2]);
					}
				});
			}

			else if(line.startsWith("MAUVAISE/")){
				Platform.runLater(new Runnable(){
					@Override public void run() {
						userActif = line.split("/")[1];
						labelNotify.setText("Solution refusee , [" + userActif + "] devient actif");
						if(userName.equals(userActif)){
							hb.getChildren().removeAll(hb.getChildren());
							hb.getChildren().addAll(labelSolution, textSolution, buttonSubmit);
							tempsResolution = 60;
							/*Demarage du timer pour la phase de resolution*/
							timerResolution = new Timer();
							timerResolution.schedule(
									new TimerTask() {

										@Override
										public void run() {
											Platform.runLater(new Runnable(){
												@Override public void run() {
													labelTimer.setText("[Temps restant] = " + (tempsResolution/60) + " min, " + (tempsResolution%60) + " sec");
													if(tempsResolution > 0)
														tempsResolution--;
												}
											});
										}
									}, 0, 1000);
						}
					}
				});
			}

			else if(line.startsWith("SASOLUTION/")){
				Platform.runLater(new Runnable(){
					@Override public void run() {
						solution = Tools.getSolution(line.split("/")[2], liste, cases, color);
						labelNotify.setText("[" + line.split("/")[1] + "] propose: " + solution);
					}
				});
			}

			/*Deplacements des robots associes a la solution propsee et si on veut une animation*/
			else if(line.startsWith("BONNE/") && animation){
				Platform.runLater(new Runnable(){
					@Override public void run() {

						if(userName.equals(userActif))
							score++;

						labelNotify.setText("La solution a ete trouvee");
						labelScore.setText("Score : " + score);

						int x = 0, y = 0;
						ImageView pawn;
						ArrayList<SeqTransition> seqT = new ArrayList<>();
						ArrayList<SequentialTransition> toPlay = new ArrayList<>();
						ArrayList<Integer> listRed = new ArrayList<>();
						ArrayList<Integer> listBlue = new ArrayList<>();
						ArrayList<Integer> listYellow = new ArrayList<>();
						ArrayList<Integer> listGreen = new ArrayList<>();
						System.out.println("On rentre ---->");
						while( !solution.isEmpty()){
							if(solution.charAt(0) == 'R')
								pawn = redPawn_img;
							else if(solution.charAt(0) == 'B')
								pawn = bluePawn_img;
							else if(solution.charAt(0) == 'J')
								pawn = yellowPawn_img;
							else
								pawn = greenPawn_img;

							String sol = "";
							int k = 2;
							while(k<solution.length() && solution.charAt(k) != 'R' && solution.charAt(k) != 'B' && solution.charAt(k) != 'J' && solution.charAt(k) != 'V'){
								sol += solution.charAt(k);
								k++;
							}


							if(solution.charAt(1) == 'H')
								y = Integer.valueOf("-"+sol);
							if(solution.charAt(1) == 'B')
								y = Integer.valueOf(sol);
							if(solution.charAt(1) == 'G')
								x = Integer.valueOf("-"+sol);
							if(solution.charAt(1) == 'D')
								x = Integer.valueOf(sol);

							seqT.add(new SeqTransition(pawn, x, y));

							solution = solution.substring(k, solution.length());

							if(pawn == redPawn_img){
								listRed.add(x);
								listRed.add(y);
							}
							if(pawn == bluePawn_img){
								listBlue.add(x);
								listBlue.add(y);
							}
							if(pawn == yellowPawn_img){
								listYellow.add(x);
								listYellow.add(y);
							}
							if(pawn == greenPawn_img){
								listGreen.add(x);
								listGreen.add(y);
							}

							x = 0;
							y = 0;
						}


						map.put(redPawn_img, listRed);
						map.put(bluePawn_img, listBlue);
						map.put(yellowPawn_img, listYellow);
						map.put(greenPawn_img, listGreen);

						boolean first_red = true;
						boolean first_blue = true;
						boolean first_yellow = true;
						boolean first_green = true;

						int posxRed = 0;
						int posyRed = 0;
						int posxBlue = 0;
						int posyBlue = 0;
						int posxYellow = 0;
						int posyYellow = 0;
						int posxGreen = 0;
						int posyGreen = 0;
						ImageView a_bouge;

						for (SeqTransition s : seqT) {
							a_bouge = s.getIv();
							if(a_bouge == redPawn_img){
								if( !first_red){
									posxRed += map.get(redPawn_img).get(0);
									posyRed += map.get(redPawn_img).get(1);
									map.get(a_bouge).remove(0);
									map.get(a_bouge).remove(0);
								}
								first_red = false;
								toPlay.add(s.seqTrans(posxRed, posyRed));
							}
							if(a_bouge == bluePawn_img){
								if( !first_blue){
									posxBlue += map.get(bluePawn_img).get(0);
									posyBlue += map.get(bluePawn_img).get(1);
									map.get(a_bouge).remove(0);
									map.get(a_bouge).remove(0);
								}
								first_blue = false;
								toPlay.add(s.seqTrans(posxBlue, posyBlue));
							}
							if(a_bouge == yellowPawn_img){
								if( !first_yellow){
									posxYellow += map.get(yellowPawn_img).get(0);
									posyYellow += map.get(yellowPawn_img).get(1);
									map.get(a_bouge).remove(0);
									map.get(a_bouge).remove(0);
								}
								first_yellow = false;
								toPlay.add(s.seqTrans(posxYellow, posyYellow));
							}
							if(a_bouge == greenPawn_img){
								if( !first_green){
									posxGreen += map.get(greenPawn_img).get(0);
									posyGreen += map.get(greenPawn_img).get(1);
									map.get(a_bouge).remove(0);
									map.get(a_bouge).remove(0);
								}
								first_green = false;
								toPlay.add(s.seqTrans(posxGreen, posyGreen));
							}
						}


						toPlay.get(0).play();
						/*Pour tout avoir sequentiellement*/
						for(int k = 0; k < seqT.size()-1; k++){
							toPlay.get(k).setOnFinished(new EventHandler<ActionEvent>() {

								@Override
								public void handle(ActionEvent event) {
									index++;
									toPlay.get(index).play();
								}
							});
						}
						index = 0;

					}
				});
			}      //Fin BONNE solution

			else if(line.startsWith("FINRESO/")){
				Platform.runLater(new Runnable(){
					@Override public void run() {
						labelNotify.setText("Plus de joueur restants, fin du tour");
						hb.getChildren().removeAll(hb.getChildren());
						if(timerResolution != null){
							timerResolution.cancel();
							labelTimer.setText("");
						}
					}
				});
			}

			else if(line.startsWith("TROPLONG/")){
				Platform.runLater(new Runnable(){
					@Override public void run() {
						userActif = line.split("/")[1];
						labelNotify.setText("Temps depasse, fin du tour. [" + userActif + "] devient actif");
						if(userName.equals(userActif)){
							hb.getChildren().removeAll(hb.getChildren());
							hb.getChildren().addAll(labelSolution, textSolution, buttonSubmit);
							tempsResolution = 60;
							/*Demarage du timer pour la phase de resolution*/
							timerResolution = new Timer();
							timerResolution.schedule(
									new TimerTask() {

										@Override
										public void run() {
											Platform.runLater(new Runnable(){
												@Override public void run() {
													labelTimer.setText("[Temps restant] =  " + (tempsResolution/60) + " min, " + (tempsResolution%60) + " sec");
													if(tempsResolution > 0)
														tempsResolution--;
												}
											});
										}
									}, 0, 1000);
						}
						else{
							hb.getChildren().removeAll(hb.getChildren());
							if(timerResolution != null){
								timerResolution.cancel();
								labelTimer.setText("");
							}
						}
					}
				});
			}

			else if(line.startsWith("VAINQUEUR/")){
				Platform.runLater(new Runnable(){
					@Override public void run() {
						hb.getChildren().removeAll(hb.getChildren());
						bilan = line.split("/")[1];
						bilan = "[Tour] = " + bilan.charAt(0) + "\n[Scores] = "+bilan.substring(1, bilan.length());
						labelBilan.setText(bilan);
						labelNotify.setText("");
						score = 0;
					}
				});
			}

			else{       //Une notification  (connexion/deconnexion d'un joueur ou un message de CHAT)
				if(line.startsWith("DECONNEXION/")){
					Platform.runLater(new Runnable(){
						@Override public void run() {
							labelNotify.setText("Le joueur " + line.split("/")[1] + " a quitté la partie");
							nbPlayers -= 1;
							labelNbJoueur.setText("[Number of players] : " + nbPlayers);
						}
					});

				}
				else if(line.startsWith("CONNECTE/")){
					Platform.runLater(new Runnable(){
						@Override public void run() {
							labelNotify.setText("Le joueur " + line.split("/")[1] + " a rejoint la partie");
							nbPlayers += 1;
							labelNbJoueur.setText("Number of players : " + nbPlayers);
						}
					});
				}
				else if(line.startsWith("CHAT/")){     //ChatMessage
					Platform.runLater(new Runnable(){
						@Override public void run() {
							System.out.println("MESSAGE : " + line);
							Date date = new Date();
							chatBox.appendText("[" + dateFormat.format(date) + "]" + "<" + line.split("/")[1] + "> : " + line.split("/")[2] + "\n");
						}
					});
				}
				else if(line.startsWith("COMPATIBLEAFFICHAGE/")){
					animation = true;
				}
				else
					System.out.println("SHOULD NEVER HAPPEND : " + line);
			}

		}
	}

}
