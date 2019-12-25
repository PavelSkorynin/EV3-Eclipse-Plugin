/**
 * Progress Windows during file upload
 */
package de.hab.ev3plugin.gui;

import ilg.gnuarmeclipse.managedbuild.cross.Activator;

import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.IMessageProvider;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.RowData;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.ProgressBar;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.eclipse.wb.swt.SWTResourceManager;

public class UploadProgress extends TitleAreaDialog {
	private Text text;
	private ProgressBar progressBar;

	public static final int DPI_CURRENT = Display.getDefault().getDPI().x;
	public static final float DPI_DEFAULT = 96.0f;
	public static final float DPI_SCALE = DPI_CURRENT / DPI_DEFAULT;
			
	/**
	 * Create the dialog.
	 * 
	 * @param parentShell
	 */
	public UploadProgress(Shell parentShell) {
		super(parentShell);
	}

	/**
	 * Create contents of the dialog.
	 * 
	 * @param parent
	 */
	@Override
	protected Control createDialogArea(Composite parent) {
		setMessage("Please hold.");
		setTitleImage(AbstractUIPlugin
				.imageDescriptorFromPlugin(Activator.PLUGIN_ID,
						"icons/EV3_Icon_small.png").createImage());
		setTitle("Uploading to EV3...");
		setDialogHelpAvailable(false);
		setHelpAvailable(false);
		parent.setToolTipText("");
		Composite area = (Composite) super.createDialogArea(parent);
		Composite container = new Composite(area, SWT.NONE);
		container.setLayoutData(new GridData(GridData.FILL_BOTH));

		progressBar = new ProgressBar(container, SWT.HORIZONTAL);
		// progressBar.setState(SWT.ERROR);
		progressBar.setBounds((int)(21 * DPI_SCALE), (int)(10 * DPI_SCALE), (int)(413 * DPI_SCALE), (int)(34 * DPI_SCALE));
		progressBar.setMinimum(0);
		progressBar.setMaximum(100);
		RowData rowData = new RowData();
		progressBar.setLayoutData(rowData);

		text = new Text(container, SWT.BORDER | SWT.READ_ONLY | SWT.V_SCROLL
				| SWT.MULTI);
		text.setEditable(false);
		text.setBounds((int)(21 * DPI_SCALE), (int)(50 * DPI_SCALE), (int)(413 * DPI_SCALE), (int)(95 * DPI_SCALE));
		return area;
	}

	/**
	 * Create contents of the button bar.
	 * 
	 * @param parent
	 */
	@Override
	protected void createButtonsForButtonBar(Composite parent) {
		Button button = createButton(parent, IDialogConstants.OK_ID, "Abort",
				true);
		button.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
			}
		});
	}

	/**
	 * Return the initial size of the dialog.
	 */
	@Override
	protected Point getInitialSize() {
		return new Point((int)(450 * DPI_SCALE), (int)(300 * DPI_SCALE));
	}
	
	@Override
	protected boolean isResizable() {
		return true;
	}

	public void setProgress(int percentage, String status) {
		progressBar.setSelection(percentage);
		setMessage(status, IMessageProvider.INFORMATION);
	}

	public void setProgress(int percentage) {
		progressBar.setSelection(percentage);
	}

	public void log(String line) {
		text.append(line);
	}

}
