#include "presentation_video.h"

#include <QFileInfo>

#include "presentationengine_video.h"
#include "videoprojectorwindow.h"
#include "presentationpropertieswidget_video.h"

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
	result->autoPlay_ = json["autoPlay"].toBool(result->autoPlay_);
	result->repeat_ = json["repeat"].toBool(result->repeat_);
	return result;
}

QJsonObject Presentation_Video::toJSON() const
{
	return QJsonObject {
		{"filename", filename_},
		{"autoPlay", autoPlay_},
		{"repeat", repeat_}
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

QPixmap Presentation_Video::specialIcon() const
{
	static QPixmap autoSlidePixmap(":/icons/16/Repeat_16px.png");
	return repeat_ ? autoSlidePixmap : QPixmap();
}

QWidget *Presentation_Video::createPropertiesWidget(QWidget *parent)
{
	return new PresentationPropertiesWidget_Video(weakPtr_.toStrongRef(), parent);
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
	return presentationEngine_video;
}

QString Presentation_Video::classIdentifier() const
{
	return "video.video";
}

void Presentation_Video::activatePresentation(int startingSlide)
{
	Q_UNUSED(startingSlide);

	if(autoPlay_)
		videoProjectorWindow->playVideo(filename_);
	else
		videoProjectorWindow->setVideo(filename_);

	videoProjectorWindow->setRepeat(repeat_);
}

void Presentation_Video::setSlide(int localSlideId, bool force)
{
	Q_UNUSED(localSlideId);
	Q_UNUSED(force);

	if(force)
		activatePresentation(0);
}

Presentation_Video::Presentation_Video()
{

}
