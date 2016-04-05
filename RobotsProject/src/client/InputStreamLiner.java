package client;

import java.io.FilterInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

/**
 * Cette classe fournit une mÃ©thode readLine() pour les InputStream.
 * Il y a en fait trois mÃ©thodes (readLineWin, readLineUnix et
 * readLineMac). En effet, selon le systÃ¨me le sÃ©parateur de ligne est :
 * - Unix : 0x0A
 * - Windows : 0x0D 0x0A
 * - Mac : 0x0D
 * 
 * Cette classe est une alternative simple
 * au couple InputStreamReader+BufferedReader.
 * De plus, ce couple pose problÃ¨me si l'on souhaite continuer
 * Ã  lire le flux en mode binaire. En effet BufferedReader :
 * - est buffÃ©risÃ© et lit en avance des octets,
 * - ces octets sont interprÃ©tÃ©s comme des caractÃ¨res en fonction
 *   d'un encodage (Ã©ventuellement par dÃ©faut, i.e. ISO-machin),
 * - il est compliquÃ© de dÃ©sinterprÃ©ter cet encodage pour retrouver
 *   les octets bruts initiaux.
 * 
 * @author Lionel Seinturier <Lionel.Seinturier@lip6.fr>
 * @version 1.0
 */
public class InputStreamLiner extends FilterInputStream {
    
    public InputStreamLiner(InputStream in) {
        super(in);
    }
    
    final static public int UNIX = 0;
    final static public int WIN = 1;
    final static public int MAC = 2;
    
    /**
     * MÃ©thode gÃ©nÃ©rale de lecture d'une ligne.
     * En fonction du paramÃ¨tre typeFichier, la lecture effective est
     * sous-traitÃ©e Ã  une des trois autres mÃ©thodes de la classe.
     * 
     * @param typeFichier  Ã©numÃ©rÃ© valant soit UNIX, soit WIN, soit MAC 
     */
    public String readLine( int typeFichier ) throws IOException {
        switch(typeFichier) {
            case UNIX : return readLineUnix();
            case WIN : return readLineWin();
            case MAC : return readLineMac();
            default :
                throw new IllegalArgumentException(
                            "Valeur "+typeFichier+" interdite");
        }
    }
    

    /**
     * Lit une ligne sous Unix
     * (i.e. jusqu'Ã  la prochaine occurrence de 0x0A).
     * 
     * @return  les octets lus (Ã  l'exclusion de 0x0A) sous forme de String
     *          null si la fin du flux est atteinte
     */
    @SuppressWarnings("unchecked")
	public String readLineUnix() throws IOException {
        
        int b = read();
        if (b==-1)  return null;
        
        buffer.clear();
        while ( b!=-1 && b!=0x0A ) {
            buffer.add(new Integer(b));
            b = read();
        }
        return bufferContent();
    }
    
    /**
     * Lit une ligne sous Windows
     * (i.e. jusqu'Ã  la prochaine occurrence de 0x0D 0x0A).
     * 
     * @return  les octets lus (Ã  l'exclusion de 0x0D 0x0A) sous forme de String
     *          null si la fin du flux est atteinte
     */
    @SuppressWarnings("unchecked")
	public String readLineWin() throws IOException {
        
        buffer.clear();
        boolean first = true;
        
        while (true) {
            
            int b = read();
            if (b==-1) {
                if (first)  return null;
                return bufferContent();
            }
            first = false;
        
            if ( b == 0x0D ) {
                b = read();
                if ( b==-1 || b==0x0A )
                    return bufferContent();
                buffer.add(new Integer(0x0D));
                buffer.add(new Integer(b));
            }
            else
                buffer.add(new Integer(b));
        }
    }
    
    /**
     * Lit une ligne sous Mac
     * (i.e. jusqu'Ã  la prochaine occurrence de 0x0D).
     * 
     * @return  les octets lus (Ã  l'exclusion de 0x0D) sous forme de String
     *          null si la fin du flux est atteinte
     */
    @SuppressWarnings("unchecked")
	public String readLineMac() throws IOException {
        
        int b = read();
        if (b==-1)  return null;
        
        buffer.clear();
        while ( b!=-1 && b!=0x0D ) {
            buffer.add(new Integer(b));
            b = read();
        }
        return bufferContent();
    }
    

    /** Buffer pour stocker les donnÃ©es de la ligne (sous forme d'Integer). */
    @SuppressWarnings("rawtypes")
	private List buffer = new ArrayList();

    /** @return  le contenu du buffer sous forme de String. */
    private String bufferContent() {
        
        @SuppressWarnings("unchecked")
		Integer[] content = (Integer[]) buffer.toArray(integerArray);
        byte[] b = new byte[content.length];
        for (int i = 0; i < b.length; i++) {
            b[i] = content[i].byteValue();
        }
        
        return new String(b);
    }
    
    /** Variable de travail pour bufferContent(). */
    final static private Integer[] integerArray = new Integer[0];
}