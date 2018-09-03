#include "presentation_video.h"

#include <QFileInfo>

#include "presentationengine_video.h"
#include "videoprojectorwindow.h"

QStringList Presentation_Video::validExtensions()
{
	static const QStringList result{
		"mp4", "mov", "wmv", "webm", "flv", "avi", "mkv", "mpg", "mpeg", "m4v"
	};
	return result;
}

QSharedPointer<Presentation_Video> Presentation_Video::createFromFilename(const QString &filename)
{
	QSharedPointer<Presentation_Video> result(new Presentation_Video);
	result->weakPtr_ = result;
	result->filename_ = filename;
	result->identification_ = QFileInfo(filename).completeBaseName();
	return result;
}

QSharedPointer<Presentation_Video> Presentation_Video::createFromJSON(const QJsonObject &json)
{
	const QString filename = json["filename"].toString();
	if(!QFile(filename).exists())
		return nullptr;

	QSharedPointer<Presentation_Video> result(new Presentation_Video);
	result->weakPtr_ = result;
	result->filename_ = filename;
	result->identification_ = QFileInfo(filename).completeBaseName();
	return result;
}

QJsonObject Presentation_Video::toJSON() const
{
	return QJsonObject {
		{"filename", filename_}
	};
}

Presentation_Video::~Presentation_Video()
{

}

QString Presentation_Video::identification() const
{
	return identification_;
}

QPixmap Presentation_Video::icon() const
{
	static QPixmap icon(":/icons/16/Play Button_16px.png");
	return icon;
}

QWidget *Presentation_Video::createPropertiesWidget(QWidget *parent)
{
	Q_UNUSED(parent);

	return nullptr;
}

int Presentation_Video::slideCount() const
{
	return 1;
}

QPixmap Presentation_Video::slideIdentificationIcon(int i) const
{
	Q_UNUSED(i);

	static QPixmap icon(":/icons/16/Play Button_16px.png");
	return icon;
}

PresentationEngine *Presentation_Video::engine() const
{
	return presentationEngine_Video;
}

QString Presentation_Video::classIdentifier() const
{
	return "video.video";
}

void Presentation_Video::activatePresentation(int startingSlide)
{
	Q_UNUSED(startingSlide);

	videoProjectorWindow->playVideo(filename_);
}

void Presentation_Video::deactivatePresentation()
{

}

void Presentation_Video::setSlide(int localSlideId, bool force)
{
	Q_UNUSED(localSlideId);
	Q_UNUSED(force);

	// TODO force
}

Presentation_Video::Presentation_Video()
{

}
