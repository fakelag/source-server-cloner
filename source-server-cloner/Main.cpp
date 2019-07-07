#include <string>
#include <cctype>

#include "FTPClient.h"
#include "Downloader.h"

bool InputConfig( std::string* psHost, std::string* psPort, std::string* psUsername, std::string* psPassword, CFTPClient::FTP_PROTOCOL* pProtocol )
{
	std::string sCorrect;

	do
	{
		std::cout << "host: "; std::cin >> *psHost;
		std::cout << "port: "; std::cin >> *psPort;
		std::cout << "username: "; std::cin >> *psUsername;
		std::cout << "password: "; std::cin >> *psPassword;

		bool bProtocolOk = false;

		while ( !bProtocolOk )
		{
			std::string sProtocol;
			std::cout << "protocol (FTP/SFTP/FTPS): "; std::cin >> sProtocol;

			std::transform( sProtocol.begin(), sProtocol.end(), sProtocol.begin(), []( char chInput ) -> char {
				return std::toupper( chInput );
			} );

			if ( sProtocol == "FTP" )
			{
				*pProtocol = CFTPClient::FTP_PROTOCOL::FTP;
				bProtocolOk = true;
			}
			else if ( sProtocol == "SFTP" )
			{
				*pProtocol = CFTPClient::FTP_PROTOCOL::SFTP;
				bProtocolOk = true;
			}
			else if ( sProtocol == "FTPS" )
			{
				*pProtocol = CFTPClient::FTP_PROTOCOL::FTPS;
				bProtocolOk = true;
			}
			else if ( sProtocol == "Q" )
			{
				return false; // quit
			}
		};

		std::cout << "Is this information correct? (y/n/q): "; std::cin >> sCorrect;

		if ( sCorrect == "q" )
			return false;

	} while ( sCorrect != "y" );

	return true;
}

int main()
{
	CFTPClient* pFTPClient = new CFTPClient( []( const std::string& log ) -> void {
		std::cout << log << std::endl;
	} );
	
	std::string sHost;
	std::string sPort;
	std::string sUsername;
	std::string sPassword;
	CFTPClient::FTP_PROTOCOL nProtocol;

	if ( !InputConfig( &sHost, &sPort, &sUsername, &sPassword, &nProtocol ) )
		return 0;

	pFTPClient->SetTimeout( 20000 ); // 20 seconds

	if ( !pFTPClient->InitSession( sHost, std::stoi( sPort ), sUsername, sPassword, nProtocol, CFTPClient::SettingsFlag::ENABLE_LOG ) )
	{
		std::cout << "[SSC] Unable to login." << std::endl;
		std::getchar();
		return 0;
	}

	while ( !CloneCStrike( pFTPClient, sHost ) )
	{
		std::string sTryAgain;
		std::cout << "Cloning failed - try again? (y/n): "; std::cin >> sTryAgain;

		if ( sTryAgain != "y" )
			break;
	}

	return 0;
}
