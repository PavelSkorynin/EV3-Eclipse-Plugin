package org.eclipse.cdt.core.templateengine.process.processes;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import org.apache.commons.io.FileUtils;
import org.eclipse.cdt.core.templateengine.TemplateCore;
import org.eclipse.cdt.core.templateengine.TemplateEngineHelper;
import org.eclipse.cdt.core.templateengine.process.ProcessArgument;
import org.eclipse.cdt.core.templateengine.process.ProcessFailureException;
import org.eclipse.cdt.core.templateengine.process.ProcessRunner;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;

public class CopyFolder extends ProcessRunner {

	/**
	 * This method Adds the list of folders to the corresponding Project.
	 */
	@Override
	public void process(TemplateCore template, ProcessArgument[] args, String processId, IProgressMonitor monitor)
			throws ProcessFailureException {
		IProject projectHandle = null;
		ProcessArgument[][] dirs = null;

		for (ProcessArgument arg : args) {
			String argName = arg.getName();
			if (argName.equals("projectName")) { //$NON-NLS-1$
				projectHandle = ResourcesPlugin.getWorkspace().getRoot().getProject(arg.getSimpleValue());
			} else if (argName.equals("dirs")) { //$NON-NLS-1$
				dirs = arg.getComplexArrayValue();
			}
		}

		if (projectHandle == null)
			throw new ProcessFailureException(
					getProcessMessage(processId, IStatus.ERROR, Messages.getString("AddFiles.8"))); //$NON-NLS-1$

		if (dirs == null)
			throw new ProcessFailureException(
					getProcessMessage(processId, IStatus.ERROR, Messages.getString("AddFiles.9"))); //$NON-NLS-1$

		for (int i = 0; i < dirs.length; i++) {
			ProcessArgument[] file = dirs[i];
			String fileSourcePath = file[0].getSimpleValue();
			String fileTargetPath = file[1].getSimpleValue();

			URL sourcePath;
			try {
				sourcePath = TemplateEngineHelper.getTemplateResourceURLRelativeToTemplate(template, fileSourcePath);
				if (sourcePath == null) {
					throw new ProcessFailureException(getProcessMessage(processId, IStatus.ERROR,
							Messages.getString("AddFiles.1") + fileSourcePath)); //$NON-NLS-1$
				}
			} catch (IOException e1) {
				throw new ProcessFailureException(Messages.getString("AddFiles.2") + fileSourcePath); //$NON-NLS-1$
			}

			try {
				String targetPath = projectHandle.getFolder(fileTargetPath).getLocation().toString();

				FileUtils.copyDirectory(new File(sourcePath.getPath()), new File(targetPath));
			} catch (Exception e) {
				throw new ProcessFailureException(Messages.getString("AddFiles.6") + e.getMessage(), e); //$NON-NLS-1$
			}

		}
		try {
			projectHandle.refreshLocal(IResource.DEPTH_INFINITE, null);
		} catch (CoreException e) {
			throw new ProcessFailureException(Messages.getString("AddFiles.7") + e.getMessage(), e); //$NON-NLS-1$
		}

	}
}
