import java.applet.Applet;
import java.awt.*;
 
public class FrMain extends Applet {
 
    // Print a message on the screen (x=20, y=10).
    public void paint(Graphics g) {
        g.drawString("Hello, world!", 20, 10);
 
        // Draws a circle on the screen (x=40, y=30).
        g.drawArc(40, 30, 20, 20, 0, 360);
    }
}
