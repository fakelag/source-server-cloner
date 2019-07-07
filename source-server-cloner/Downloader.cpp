#include <filesystem>
#include "FTPClient.h"
#include "Downloader.h"

bool FindGameRoot( CFTPClient* pFTPClient, const std::string& sGameFolder, std::string* psRootPath )
{
	std::string sDirList;
	if ( !pFTPClient->List( "/", sDirList, true ) )
		return false;

	bool bFound = false;
	std::istringstream directoryStream;
	directoryStream.str( sDirList );

	std::string sLine;
	while ( std::getline( directoryStream, sLine, '\r' ) )
	{
		if ( sLine == "\n" + sGameFolder )
		{
			*psRootPath = sGameFolder;
			bFound = true;
			break;
		}
	}

	if ( !bFound )
		return false;

	return true;
}

void CreateLocalPaths( const std::string& sLocalRoot, const std::string& sRelativePath )
{
	std::istringstream pathStream;
	pathStream.str( sRelativePath );

	std::vector<std::string> vecDepth;
	std::string sLine;

	while ( std::getline( pathStream, sLine, '/' ) )
		vecDepth.push_back( sLine );

	vecDepth.pop_back(); // pop the last element (either a wildcard or filename)

	std::experimental::filesystem::current_path();

	if ( vecDepth.size() > 0 )
	{
		// create local folder hierarchy
		std::string sFullPath = std::experimental::filesystem::current_path().string() + "\\" + sLocalRoot;
		_mkdir( sFullPath.c_str() );

		for ( std::string dir : vecDepth )
		{
			sFullPath = sFullPath + "\\" + dir;
			_mkdir( sFullPath.c_str() );
		}
	}
}

bool DownloadFile( CFTPClient* pFTPClient, const std::string& sLocalRoot, const std::string& sRelativePath )
{
	CreateLocalPaths( sLocalRoot, sRelativePath );

	std::cout << "[FTP] Downloading File: " + sRelativePath << std::endl;
	return pFTPClient->DownloadFile( sLocalRoot + "\\" + sRelativePath, sRelativePath );
}

bool DownloadDirectory( CFTPClient* pFTPClient, const std::string& sLocalRoot, const std::string& sRelativePath )
{
	CreateLocalPaths( sLocalRoot, sRelativePath + "/*" );

	std::cout << "[FTP] Downloading Directory: " + sRelativePath << std::endl;
	return pFTPClient->DownloadWildcard( sLocalRoot + "\\" + sRelativePath, sRelativePath + "/*" );
}

bool CloneGameFolder( CFTPClient* pFTPClient, const std::string& sGameFolder, const std::string& sFolderName )
{
	// Find cstrike folder

	std::string sRootPath;
	if ( !FindGameRoot( pFTPClient, sGameFolder, &sRootPath ) )
	{
		std::cout << "Error: game root folder \"" + sGameFolder + "\" not found." << std::endl;
		return false;
	}

	// cstrike is present

	if ( !DownloadFile( pFTPClient, sFolderName, sRootPath + "/motd.txt" ) )
		return false;

	if ( !DownloadFile( pFTPClient, sFolderName, sRootPath + "/cfg/mapcycle.txt" ) )
		return false;

	if ( !DownloadFile( pFTPClient, sFolderName, sRootPath + "/cfg/server.cfg" ) )
		return false;

	if ( !DownloadFile( pFTPClient, sFolderName, sRootPath + "/cfg/banned_user.cfg" ) )
		return false;

	if ( !DownloadDirectory( pFTPClient, sFolderName, sRootPath + "/cfg/sourcemod" ) )
		return false;

	if ( !DownloadDirectory( pFTPClient, sFolderName, sRootPath + "/maps" ) )
		return false;

	if ( !DownloadDirectory( pFTPClient, sFolderName, sRootPath + "/custom" ) )
		return false;

	if ( !DownloadDirectory( pFTPClient, sFolderName, sRootPath + "/sound" ) )
		return false;

	if ( !DownloadDirectory( pFTPClient, sFolderName, sRootPath + "/addons" ) )
		return false;

	return true;
}