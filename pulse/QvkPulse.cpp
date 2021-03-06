
#include "QvkPulse.h"

#include <QCheckBox>
#include <QProcess>
#include <QDebug>

QvkPulse::QvkPulse()
{
}

QvkPulse::~QvkPulse()
{
}

/**
 * Returns xte checked pulse devicename
 */
QString QvkPulse::getPulseDeviceName( int value, QWidget *Pulseframe )
{
  QList<QCheckBox *> listQFrame = Pulseframe->findChildren<QCheckBox *>();
  QCheckBox *inBox;
  int x = 1;
  QString name;
  
  for ( int i = 0; i < listQFrame.count(); i++ )
  {
    inBox = listQFrame.at( i );
    if ( inBox->checkState() == Qt::Checked  )
    {
      if  ( x == value )
        name = inBox->text();
      x++;
    }
  }
  return name;
}


/**
 *  Returns count checked pulse devices
 */
int QvkPulse::getCountCheckedPulseDevices( QWidget *Pulseframe )
{
  QList<QCheckBox *> listQFrame = Pulseframe->findChildren<QCheckBox *>();
  QCheckBox *inBox;
  int x = 0;
  for ( int i = 0; i < listQFrame.count(); i++ )
  {
    inBox = listQFrame.at( i );
    if ( inBox->checkState() == Qt::Checked  )
      x++;
  }
  return x;
}


void QvkPulse::pulseUnloadModule()
{
  qDebug() << "[vokoscreen] ---Begin Pulse unload Module---";
  QProcess Process;
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "c");
  Process.setProcessEnvironment(env);
  Process.start( "pactl list" );
  Process.waitForFinished();
  QString output = Process.readAllStandardOutput();
  Process.close();
  
  QStringList list = output.split( "\n" );
  
  QStringList modulNumberList;
  QString str;
  QString strLog;
  for ( int i = 0; i < list.count(); i++ )
  {
     str = list[ i ];
     if ( str.contains("vokoscreenMix", Qt::CaseInsensitive) )
     {
       str = list[ i - 2 ];
       if ( str.contains("Module", Qt::CaseInsensitive) )
       {
	 str.replace("Module #", "");
	 str.replace("\n", "");
	 modulNumberList.insert ( 0, str );
       }
     }
  }
  
  for ( int i = 0; i < modulNumberList.count(); i++ )
  {
    Process.start("pactl unload-module " + modulNumberList[ i ] );
    Process.waitForFinished();
    Process.close();
    qDebug() << "[vokoscreen] pactl unload-module number" << modulNumberList[ i ] ;
  }
  qDebug() << "[vokoscreen] ---End Pulse unload Module---";
  qDebug( " " );
}

/**
 * Return checked Pulse Device
 */
const QString QvkPulse::myPulseDevice( QWidget *Pulseframe )
{
  QList<QCheckBox *> listQFrame = Pulseframe->findChildren<QCheckBox *>();
  QCheckBox *box;
  QList<int> integerList;
  QString ret;
  
  for ( int i = 0; i < listQFrame.count(); i++ )
  {
    box = listQFrame.at( i );
    if ( box->checkState() == Qt::Checked  )
      integerList.append( i );
  }

  if ( integerList.count() == 0 )
    ret = "";

  if ( integerList.count() == 1 )
  {
    box = listQFrame[ integerList[ 0 ] ];
    ret = box->accessibleName();
  }

  if ( integerList.count() > 1 )
    ret = "vokoscreenMix.monitor";

  return ret;
}


/**
 * Get Pulse Devicname
 */
QString QvkPulse::getPulseInputDevices( int value )
{
  QProcess Process;;
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "c");
  Process.setProcessEnvironment(env);
  Process.start( "pactl list" );
  Process.waitForFinished();
  QString output = Process.readAllStandardOutput();
  Process.close();
 
  QStringList list = output.split( "\n" );

  QStringList result = list.filter( "Source #", Qt::CaseInsensitive );
  QString SourceNummber = result[ value - 1 ];

  int index = list.indexOf( SourceNummber );
  QString device = list[ index + 2 ];
  QStringList resultList = device.split( ":" );
  device = resultList[ 1 ].trimmed();
  return device;
}


/**
 * 
 */
QString QvkPulse::getPulseInputName( int value )
{
  QProcess Process;
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "c");
  Process.setProcessEnvironment(env);
  Process.start( "pactl list" );
  Process.waitForFinished();
  QString output = QString::fromUtf8(Process.readAllStandardOutput());
  Process.close();
  
  QStringList list = output.split( "\n" );

  QStringList result = list.filter( "Source #", Qt::CaseInsensitive );
  QString SourceNummber = result[ value - 1 ];
  
  int index = list.indexOf( SourceNummber );
  QString name = list[ index + 3 ];
  QStringList resultList = name.split( ":" );
  name = resultList[ 1 ].trimmed();
  return name;
}


/**
 * 
 */
int QvkPulse::getPulseInputDevicesCount()
{
  QProcess Process;
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "c");
  Process.setProcessEnvironment(env);
  Process.start( "pactl list" );
  Process.waitForFinished();
  QString output = Process.readAllStandardOutput();
  Process.close();
  
  QStringList list = output.split( "\n" );

  QStringList result = list.filter( "Source #", Qt::CaseInsensitive );
  return result.count();
}

//***************************************** New New New ***********************************************
//********************************** Only for icon in Audio Widget ******************************************
/**
 * grep "pactl list sources" to QStringList listPactlOutput
 */
void QvkPulse::grepPactlOutput()
{
  QProcess Process;
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "c");
  Process.setProcessEnvironment(env);
  Process.start( "pactl list sources" );
  Process.waitForFinished();
  QString output = QString::fromUtf8( Process.readAllStandardOutput() );
  Process.close();
  
  if ( !output.isEmpty() )
  {
    listPactlOutput.clear();
    listPactlOutput = output.split( "\n" );
  }
}


/**
 * If unplug and then plugin, the "Source #x" changed
 * we want the correct Sourcename.
 */
QString QvkPulse::getNameOfSourceNumber( int value )
{
  QStringList list = listPactlOutput.filter( "Source #" );
  return list[ value ];
}


/**
 * 
 */
QStringList QvkPulse::grepListSourceNumberX( int value )
{
  // Search first line from "Source #x"
  int posBegin = listPactlOutput.indexOf( getNameOfSourceNumber( value ) );
  
  // Search last line from "Source #x"
  int posEnd = listPactlOutput.indexOf( "", posBegin );
  
  QStringList listOfSource;
  for ( int i = posBegin; i < posEnd; i++ )
    listOfSource << listPactlOutput.at( i );
  
  return listOfSource;
}


QString QvkPulse::getIconName( int value )
{
  QString ret = "audio-input-microphone";

  if ( !listPactlOutput.empty() )
  {
    QStringList list = grepListSourceNumberX( value );
    QString iconName = list.filter( "device.icon_name" )[0].trimmed();
    iconName = iconName.split('"')[ 1 ];
  
    if ( iconName == "audio-card-pci" )
      ret = "audio-card";

    if ( iconName == "camera-web-usb" )
      ret = "camera-web";
  } 
  
  return ret;
}
