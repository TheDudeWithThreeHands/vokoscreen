#include "QvkVersion.h"

QvkVersion::QvkVersion()
{
    connect( &manager, SIGNAL( finished( QNetworkReply* ) ), SLOT( downloadFinished( QNetworkReply* ) ) );
}


void QvkVersion::doDownload()
{
    QNetworkRequest request( QUrl( "http://kohaupt-online.de/hp/version/VERSION" ) );
    
    QNetworkReply *reply = manager.get( request );
    currentDownloadsQList.append( reply );
}


bool QvkVersion::saveToDisk( const QString &remoteFilename, QIODevice *data )
{
    QFile remoteFile( QStandardPaths::writableLocation( QStandardPaths::TempLocation ) +
		      QDir::separator() +
		      remoteFilename );

    if ( !remoteFile.open( QIODevice::WriteOnly ) )
    {
        fprintf( stderr, "Could not open %s for writing: %s\n", qPrintable( remoteFilename ), qPrintable( remoteFile.errorString() ) );
        return false;
    }

    remoteFile.write( data->readAll() );
    remoteFile.close();
    return true;
}


void QvkVersion::downloadFinished( QNetworkReply *reply )
{
    QString localVersionFilename = "vokoscreenVERSION";
  
    QUrl url = reply->url();
    if ( reply->error() )
    {
        fprintf( stderr, "[vokoscreen] Download of %s failed: %s\n", url.toEncoded().constData(), qPrintable( reply->errorString() ) );
    }
    else
    {
       if ( saveToDisk( localVersionFilename, reply ) )
       {
            printf( "[vokoscreen] File VERSION available\n" );
	    qDebug( " " );
       }
    }

    readVersionTempFile( localVersionFilename );

    emit versionDownloadFinish();
}


void QvkVersion::readVersionTempFile( QString localVersionFilename )
{
    QSettings settings( QStandardPaths::writableLocation( QStandardPaths::TempLocation ) + 
                        QDir::separator() +
			localVersionFilename, QSettings::IniFormat );

    settings.beginGroup( "Info" );
       setRemoteVersion( settings.value( "Version" ).toString() );
    settings.endGroup();
}


void QvkVersion::setRemoteVersion( QString version )
{
    remoteVersion = version;
}


QString QvkVersion::getRemoteVersion()
{
    return remoteVersion; 
}


bool QvkVersion::isNewVersionAvailable( QString localVersion, QString remoteVersion )
{
  if ( remoteVersion > localVersion )
    return true;
  else
    return false;
}