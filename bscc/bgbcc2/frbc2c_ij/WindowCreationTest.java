/*
 * Copyright (c) 2002-2008 LWJGL Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * * Neither the name of 'LWJGL' nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// package org.lwjgl.test;

import org.lwjgl.LWJGLException;
import org.lwjgl.input.Keyboard;
import org.lwjgl.opengl.Display;
import org.lwjgl.opengl.DisplayMode;
import org.lwjgl.opengl.GL11;

import static org.lwjgl.opengl.GL11.*;

/**
 * Small class for testing that the Window is creatable
 * If this class can't run, LWJGL wont work!
 *
 * @author Brian Matzon <brian@matzon.dk>
 */
public class WindowCreationTest {

	/** Locatable modes */
	private DisplayMode[]	located_modes;

	/** Fixed selectable modes */
	private DisplayMode[]	fixed_modes = new DisplayMode[10];


	/** Window position x */
	private int						window_x;

	/** Window position y */
	private int						window_y;

	/** Color being cleared to */
	private float					color				= 0f;

	/** Direction moving clearing color */
	private int						direction		= 1;

	/** Whether we're running */
	private boolean				running;

	/** Whether we're in fullscreen mode */
	private boolean				fullscreen;

	private boolean key_shift;
	private boolean key_ctrl;
	private boolean key_alt;
	boolean[] keymap;

	ConsoleGL con;

	/**
	 * Initializes the test
	 * @return true if initialization was successfull
	 */
	public boolean initialize() {
		try {
			// get available modes, and print out
			located_modes = Display.getAvailableDisplayModes();
			System.out.println("Found " + located_modes.length + " display modes");

			// get 640x480, 800x600, 1024x768 modes
			findFixedModes();

			// create default windowed display 640*480 @ 100, 100
			setDefaultDisplayMode();

			window_x = window_y = 100;
			Display.setLocation(window_x, window_y);

			Display.create();
			
			con=new ConsoleGL(640, 480);
			keymap=new boolean[256];
			
			return true;
		} catch (LWJGLException le) {
			le.printStackTrace();
		}
		return false;
	}

	/** Locate fixed modes */
	private void findFixedModes() {
		// get 640*480 modes
		fixed_modes[0] = getDisplayMode(640, 480, 16, -1);
		fixed_modes[1] = getDisplayMode(640, 480, 24, -1);
		fixed_modes[2] = getDisplayMode(640, 480, 32, -1);

		// get 800*600*16*60
		fixed_modes[3] = getDisplayMode(800, 600, 16, -1);
		fixed_modes[4] = getDisplayMode(800, 600, 24, -1);
		fixed_modes[5] = getDisplayMode(800, 600, 32, -1);

		// get 1024*768*16*60
		fixed_modes[6] = getDisplayMode(1024, 768, 16, -1);
		fixed_modes[7] = getDisplayMode(1024, 768, 24, -1);
		fixed_modes[8] = getDisplayMode(1024, 768, 32, -1);
	}

	/**
	 * Executes the test
	 */
	private void execute() {
		running = true;

		// wait for user to close window
		while (!Display.isCloseRequested() && running) {

			// handle input accordingly
			handleInput();

			con.updateTick(0.1);
			
			// render something
			render();

			// update display as needed
			Display.update();

			// no need to run at full speed
			try {
				Thread.sleep(100);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	/**
	 * Destroys any resources used while running test
	 */
	public void destroy() {
		// nuke window and get out
		Display.destroy();
	}

	private void keyChar(char key, boolean down)
	{
		if(down)
			con.emitChar(key);
	}

	/**
	 * Handles the input
	 */
	private void handleInput()
	{
		boolean down;
		
		while (Keyboard.next())
		{
			down=Keyboard.getEventKeyState();

			if(down)
				{ keymap[Keyboard.getEventKey()]=true; }
			else
				{ keymap[Keyboard.getEventKey()]=false; }
				
			if(keymap[Keyboard.KEY_LSHIFT] || keymap[Keyboard.KEY_RSHIFT])
				{ key_shift=true; } else { key_shift=false; }
			if(keymap[Keyboard.KEY_LCONTROL] || keymap[Keyboard.KEY_RCONTROL])
				{ key_ctrl=true; } else { key_ctrl=false; }
			if(keymap[Keyboard.KEY_LMENU] || keymap[Keyboard.KEY_RMENU])
				{ key_alt=true; } else { key_alt=false; }
			
			// we only want key down events
			//if (!Keyboard.getEventKeyState()) {
			//	continue;
			//}

			// check for exit
			if (Keyboard.getEventKey() == Keyboard.KEY_ESCAPE) {
				running = false;
			}

			switch(Keyboard.getEventKey())
			{
			case Keyboard.KEY_0: keyChar(key_shift?')':'0', down); break;
			case Keyboard.KEY_1: keyChar(key_shift?'!':'1', down); break;
			case Keyboard.KEY_2: keyChar(key_shift?'@':'2', down); break;
			case Keyboard.KEY_3: keyChar(key_shift?'#':'3', down); break;
			case Keyboard.KEY_4: keyChar(key_shift?'$':'4', down); break;
			case Keyboard.KEY_5: keyChar(key_shift?'%':'5', down); break;
			case Keyboard.KEY_6: keyChar(key_shift?'^':'6', down); break;
			case Keyboard.KEY_7: keyChar(key_shift?'&':'7', down); break;
			case Keyboard.KEY_8: keyChar(key_shift?'*':'8', down); break;
			case Keyboard.KEY_9: keyChar(key_shift?'(':'9', down); break;

			case Keyboard.KEY_Q: keyChar(key_shift?'Q':'q', down); break;
			case Keyboard.KEY_W: keyChar(key_shift?'W':'w', down); break;
			case Keyboard.KEY_E: keyChar(key_shift?'E':'e', down); break;
			case Keyboard.KEY_R: keyChar(key_shift?'R':'r', down); break;
			case Keyboard.KEY_T: keyChar(key_shift?'T':'t', down); break;
			case Keyboard.KEY_Y: keyChar(key_shift?'Y':'y', down); break;
			case Keyboard.KEY_U: keyChar(key_shift?'U':'u', down); break;
			case Keyboard.KEY_I: keyChar(key_shift?'I':'i', down); break;
			case Keyboard.KEY_O: keyChar(key_shift?'O':'o', down); break;
			case Keyboard.KEY_P: keyChar(key_shift?'P':'p', down); break;

			case Keyboard.KEY_A: keyChar(key_shift?'A':'a', down); break;
			case Keyboard.KEY_S: keyChar(key_shift?'S':'s', down); break;
			case Keyboard.KEY_D: keyChar(key_shift?'D':'d', down); break;
			case Keyboard.KEY_F: keyChar(key_shift?'F':'f', down); break;
			case Keyboard.KEY_G: keyChar(key_shift?'G':'g', down); break;
			case Keyboard.KEY_H: keyChar(key_shift?'H':'h', down); break;
			case Keyboard.KEY_J: keyChar(key_shift?'J':'j', down); break;
			case Keyboard.KEY_K: keyChar(key_shift?'K':'k', down); break;
			case Keyboard.KEY_L: keyChar(key_shift?'L':'l', down); break;

			case Keyboard.KEY_Z: keyChar(key_shift?'Z':'z', down); break;
			case Keyboard.KEY_X: keyChar(key_shift?'X':'x', down); break;
			case Keyboard.KEY_C: keyChar(key_shift?'C':'c', down); break;
			case Keyboard.KEY_V: keyChar(key_shift?'V':'v', down); break;
			case Keyboard.KEY_B: keyChar(key_shift?'B':'b', down); break;
			case Keyboard.KEY_N: keyChar(key_shift?'N':'n', down); break;
			case Keyboard.KEY_M: keyChar(key_shift?'M':'m', down); break;
			
			case Keyboard.KEY_MINUS:
				keyChar(key_shift?'_':'-', down); break;
			case Keyboard.KEY_EQUALS:
				keyChar(key_shift?'+':'=', down); break;
			case Keyboard.KEY_BACK:
				keyChar('\b', down); break;
			case Keyboard.KEY_TAB:
				keyChar('\t', down); break;

			case Keyboard.KEY_LBRACKET:
				keyChar(key_shift?'{':'[', down); break;
			case Keyboard.KEY_RBRACKET:
				keyChar(key_shift?'}':']', down); break;
			case Keyboard.KEY_RETURN:
				keyChar('\n', down); break;

			case Keyboard.KEY_SEMICOLON:
				keyChar(key_shift?':':';', down); break;
			case Keyboard.KEY_APOSTROPHE:
				keyChar(key_shift?'\"':'\'', down); break;
			case Keyboard.KEY_GRAVE:
				keyChar(key_shift?'~':'`', down); break;
			case Keyboard.KEY_BACKSLASH:
				keyChar(key_shift?'|':'\\', down); break;
			case Keyboard.KEY_COMMA:
				keyChar(key_shift?'<':',', down); break;
			case Keyboard.KEY_PERIOD:
				keyChar(key_shift?'>':'.', down); break;
			case Keyboard.KEY_SLASH:
				keyChar(key_shift?'?':'/', down); break;

			case Keyboard.KEY_SPACE:
				keyChar(' ', down); break;

			default: break;
			}

/*
			// check for listing of modes
			if (Keyboard.getEventKey() == Keyboard.KEY_L) {
				for(int i=0;i<fixed_modes.length; i++) {
					System.out.println("[" + i + "]: " + fixed_modes[i]);
				}
			}

			// check for display mode
			// ================================
			if (Keyboard.getEventKey() == Keyboard.KEY_0) { setMode(0); }
			if (Keyboard.getEventKey() == Keyboard.KEY_1) { setMode(1); }
			if (Keyboard.getEventKey() == Keyboard.KEY_2) {	setMode(2); }
			if (Keyboard.getEventKey() == Keyboard.KEY_3) {	setMode(3); }
			if (Keyboard.getEventKey() == Keyboard.KEY_4) {	setMode(4); }
			if (Keyboard.getEventKey() == Keyboard.KEY_5) {	setMode(5); }
			if (Keyboard.getEventKey() == Keyboard.KEY_6) {	setMode(6); }
			if (Keyboard.getEventKey() == Keyboard.KEY_7) {	setMode(7); }
			if (Keyboard.getEventKey() == Keyboard.KEY_8) {	setMode(8); }
			// --------------------------------

			// check for window move
			// ================================
			if (Keyboard.getEventKey() == Keyboard.KEY_LEFT) {
				if (!Display.isFullscreen()) {
					Display.setLocation(window_x -= 10, window_y);
				}
			}

			if (Keyboard.getEventKey() == Keyboard.KEY_RIGHT) {
				if (!Display.isFullscreen()) {
					Display.setLocation(window_x += 10, window_y);
				}
			}

			if (Keyboard.getEventKey() == Keyboard.KEY_UP) {
				if (!Display.isFullscreen()) {
					Display.setLocation(window_x, window_y -= 10);
				}
			}

			if (Keyboard.getEventKey() == Keyboard.KEY_DOWN) {
				if (!Display.isFullscreen()) {
					Display.setLocation(window_x, window_y += 10);
				}
			}
			// --------------------------------

			// check for fullscreen
			if (Keyboard.getEventKey() == Keyboard.KEY_F) {
				try {
					if (Keyboard.isKeyDown(Keyboard.KEY_LSHIFT) ||
							Keyboard.isKeyDown(Keyboard.KEY_RSHIFT)) {
						System.out.println("Performing Display.destroy()/create() cycle");
						DisplayMode saved_mode = Display.getDisplayMode();
						Display.destroy();
						Display.setDisplayMode(saved_mode);
						Display.setFullscreen(fullscreen = !fullscreen);
						Display.create();
					} else
						Display.setFullscreen(fullscreen = !fullscreen);
				} catch (LWJGLException lwjgle) {
					lwjgle.printStackTrace();
				}
			}
			*/
		}
	}

	private void setMode(int mode) {
		if(fixed_modes[mode] == null) {
			System.out.println("Unable to set mode. Not valid: " + mode);
			return;
		}

		try {
			Display.setDisplayMode(fixed_modes[mode]);
		} catch (LWJGLException le) {
			le.printStackTrace();
			System.out.println("Exception while setting mode: " + fixed_modes[mode]);
		}
	}

	/**
	 *
	 */
	private void render() {
		glClearColor(color, color, color, 1f);
		glClear(GL_COLOR_BUFFER_BIT);

		color += direction * .05f;

		if (color > 1f) {
			color = 1f;
			direction = -1 * direction;
		} else if (color < 0f) {
			direction = -1 * direction;
			color = 0f;
		}
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glOrtho(-320, 320, -240, 240, -9000, 9000);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		con.render();

		glDisable(GL_TEXTURE_2D);


		glBegin(GL_TRIANGLES);

		glTexCoord2d(0.0, 0.0);
		glColor4d(1.0, 0.0, 0.0, 1.0);
		glVertex2d(-180.0, -120.0);

		glTexCoord2d(1.0, 0.0);
		glColor4d(0.0, 0.0, 1.0, 1.0);
		glVertex2d(180.0, -120.0);

		glTexCoord2d(0.5, 1.0);
		glColor4d(1.0, 0.0, 1.0, 1.0);
		glVertex2d(0.0, 120.0);

		glEnd();
	}

	/**
	 * Main entry point
	 *
	 * @param args ignored params to app
	 */
	public static void main(String[] args) throws LWJGLException {

		System.out.println("The following keys are available:\n" +
		                   "ESCAPE:\t\tExit test\n" +
		                   "ARROW Keys:\tMove window when in non-fullscreen mode\n" +
		                   "L:\t\tList selectable display modes\n" +
		                   "0-8:\t\tSelection of display modes\n" +
		                   "F:\t\tToggle fullscreen\n" +
		                   "SHIFT-F:\tToggle fullscreen with Display.destroy()/create() cycle");

		WindowCreationTest wct = new WindowCreationTest();
		if (wct.initialize()) {
			wct.execute();
			wct.destroy();
		}
		System.exit(0);
	}

	/**
	 * Sets the display mode for fullscreen mode
	 */
	protected boolean setDefaultDisplayMode() {
		try {
			// get modes
			DisplayMode[] dm = org.lwjgl.util.Display.getAvailableDisplayModes(640, 480, -1, -1, -1, -1, -1, -1);

			org.lwjgl.util.Display.setDisplayMode(dm, new String[] { "width=" + 640, "height=" + 480, "freq=" + 60,
					"bpp=" + org.lwjgl.opengl.Display.getDisplayMode().getBitsPerPixel()});
			return true;
		} catch (Exception e) {
			e.printStackTrace();
		}
		return false;
	}

	/**
	 * Gets a specific display mode
	 */
	private DisplayMode getDisplayMode(int width, int height, int bpp, int freq) {
		DisplayMode[] dm = null;
		try {
			dm = org.lwjgl.util.Display.getAvailableDisplayModes(width, height, width, height, bpp, bpp, freq, freq);
			if(dm == null || dm.length == 0) {
				System.out.println("Problem retrieving mode with " + width + "x" + height + "x" + bpp + "@" + freq);
			}
		} catch (LWJGLException le) {
			le.printStackTrace();
			System.out.println("Problem retrieving mode with " + width + "x" + height + "x" + bpp + "@" + freq);
		}
		return (dm != null && dm.length != 0) ? dm[0] : null;
	}
}
