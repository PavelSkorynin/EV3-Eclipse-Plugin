package de.hab.ev3plugin.gui;

import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;

public class SetBluetoothCOM extends TitleAreaDialog {
	private static final int SET_ID = IDialogConstants.CLIENT_ID + 1;

	public static final int DPI_CURRENT = Display.getDefault().getDPI().x;
	public static final float DPI_DEFAULT = 96.0f;
	public static final float DPI_SCALE = DPI_CURRENT / DPI_DEFAULT;
			
	public interface Handler {
		void setValue(String name);
	}
	Handler handler;
	Composite parent;
	public void setHandler(Handler handler) { this.handler = handler; }

	private Text txtCom;
	private static String startVal = "";

	/**
	 * Create the dialog.
	 * @param parentShell
	 */
	public SetBluetoothCOM(Shell parentShell) {
		super(parentShell);
	}

	/**
	 * Create contents of the dialog.
	 * @param parent
	 */
	@Override
	protected Control createDialogArea(Composite parent) {
		this.parent = parent.getParent().getParent(); // طلع سلسفيل أهله ;-)
		setMessage("This is usually of the form COMx (e.g. COM1)");
		setTitle("Set bluetooth device name");
		Composite area = (Composite) super.createDialogArea(parent);
		Composite container = new Composite(area, SWT.NONE);
		container.setLayoutData(new GridData(GridData.FILL_BOTH));
		
		txtCom = new Text(container, SWT.BORDER);
		txtCom.setTextDirection(0);
		txtCom.setText(startVal);
		txtCom.setBounds((int)(29 * DPI_SCALE), (int)(10 * DPI_SCALE), (int)(283 * DPI_SCALE), (int)(19 * DPI_SCALE));

		return area;
	}

	/**
	 * Create contents of the button bar.
	 * @param parent
	 */
	@Override
	protected void createButtonsForButtonBar(Composite _parent) {
		Button btnSet = createButton(_parent, SET_ID, IDialogConstants.OK_LABEL,
				true);
		btnSet.setText("Set");
		btnSet.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				handler.setValue(txtCom.getText());
				parent.setVisible(false);
				parent.dispose();
			}
		});
		createButton(_parent, IDialogConstants.CANCEL_ID,
				IDialogConstants.CANCEL_LABEL, false);
	}

	/**
	 * Return the initial size of the dialog.
	 */
	@Override
	protected Point getInitialSize() {
		return new Point((int)(341 * DPI_SCALE), (int)(179 * DPI_SCALE));
	}
	
	@Override
	protected boolean isResizable() {
		return true;
	}
	/**
	 * Set InputText value
	 */
	public void setStandardValue(String val)
	{
		startVal = val;
	}

}
