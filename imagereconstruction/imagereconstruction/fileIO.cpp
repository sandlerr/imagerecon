#include "stdafx.h"
#include "fileIO.h"
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <tiffio.h>
#include <string>

HANDLE findFile(char type)
{
	COMDLG_FILTERSPEC aFileTypes [] = { { L"", L"" }, { L"All files", L"*.*" } };
	switch (type)
	{
		case 'a':
			aFileTypes[0] = { L"Text files", L"*.txt" };
			break;
		default:
			break;
	}
	
	HANDLE hFile = NULL;
	//HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	HRESULT hr = CoInitializeEx (NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog *pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{

			pFileOpen->SetFileTypes(_countof(aFileTypes), aFileTypes);
			// Show the Open dialog box.
			hr = pFileOpen->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem *pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						MessageBox(NULL, pszFilePath, L"File Path", MB_OK);
						hFile = CreateFile(pszFilePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}
		return hFile;
}

TIFF* findTifFile(char type)
{
	COMDLG_FILTERSPEC aFileTypes [] = { { L"Tiff files", L"*.tif;*.tiff" }, { L"All files", L"*.*" } };

	TIFF* tif = NULL;
	//HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog *pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{

			pFileOpen->SetFileTypes(_countof(aFileTypes), aFileTypes);
			// Show the Open dialog box.
			hr = pFileOpen->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem *pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;

					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						MessageBox(NULL, pszFilePath, L"File Path", MB_OK);
						std::wstring filePath(pszFilePath);
						int size = filePath.size();
						char * szBuffer = new char[size];
						WideCharToMultiByte(CP_ACP,0,pszFilePath,-1,szBuffer,size,NULL,NULL);
						tif = TIFFOpen(szBuffer, &type);
						CoTaskMemFree(pszFilePath);
						delete szBuffer;
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}
	return tif;
}

