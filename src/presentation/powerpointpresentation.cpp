#include "powerpointpresentation.h"

#include <QAxObject>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QRegularExpression>
#include <QStandardPaths>

#include <QDebug>

#include "gui/splashscreen.h"
#include "job/activexjobthread.h"
#include "util/standarddialogs.h"
#include "util/scopeexit.h"

const QStringList PowerpointPresentation::allowedExtensions {"ppt", "pptx"};

QSharedPointer<PowerpointPresentation> PowerpointPresentation::create(const QString &filename)
{
	QSharedPointer<PowerpointPresentation> result_;

	splashscreen->asyncAction(tr("Načítání '%1'").arg(QFileInfo(filename).fileName()), true, *activeXJobThread, [&]{
		QSharedPointer<PowerpointPresentation> result(new PowerpointPresentation());

		result->filePath_ = filename;
		result->identification_ = QFileInfo(filename).completeBaseName();

		QAxObject obj;
		if(!obj.setControl("PowerPoint.Application"))
			return standardErrorDialog(tr("Program nedetekoval instalaci PowerPointu. Bez nainstalového PowerPointu nelze pracovat s powerpointovými prezentacemi."));

		if(splashscreen->isStornoPressed())
			return;

		auto presentations = obj.querySubObject("Presentations");
		auto presentation = presentations->querySubObject(
					"Open(QString,Office::MsoTriState,Office::MsoTriState,Office::MsoTriState)",
					QDir::toNativeSeparators(filename), true, false, false
					);

		if(!presentation)
			return standardErrorDialog(tr("Nepodařilo se načíst prezentaci '%1'.").arg(filename));

		if(splashscreen->isStornoPressed())
			return;

		//SCOPE_EXIT(presentation->dynamicCall("Quit()"));

		auto slides = presentation->querySubObject("Slides");
		const int slideCount = slides->property("Count").toInt();

		const QDir tmpDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation));

		for(int slideI = 1; slideI <= slideCount; slideI++) {
			if(splashscreen->isStornoPressed())
				return;

			splashscreen->setProgress(slideI, slideCount);

			auto slide = slides->querySubObject("Item(QVariant)", slideI);
			auto transition = slide->querySubObject("SlideShowTransition");

			if(transition->property("Hidden").toInt() == (int) Office::MsoTriState::msoTrue)
				continue;

			// Obtain slide text
			{
				QString slideText;

				auto shapes = slide->querySubObject("Shapes");
				int shapeCount = shapes->property("Count").toInt();

				for(int shapeI = 1; shapeI <= shapeCount; shapeI++) {
					auto shape = shapes->querySubObject("Item(QVariant)", shapeI);

					if(shape->property("HasTextFrame").toInt() == (int) Office::MsoTriState::msoFalse)
						continue;

					auto textFrame = shape->querySubObject("TextFrame");
					auto textRange = textFrame->querySubObject("TextRange");

					slideText.append(' ');
					slideText.append(textRange->property("Text").toString());
				}

				slideText.replace(QRegularExpression("\\s+"), " ");

				if(slideText.length() > maxDescriptionLength) {
					slideText.resize(maxDescriptionLength - 3);
					slideText.append("...");
				}

				result->slideTexts_.append(slideText);
			}

			// Obtain slide image
			do {
				const QString filename = QDir::toNativeSeparators(tmpDir.absoluteFilePath("strawLumenThumbTmp.png"));

				slide->dynamicCall("Export(QString,QString,Long,Long)", filename, "PNG", 512, 512).toBool();
				result->slideThumbnails_.append(QImage(filename));
				QFile(filename).remove();

			} while(false);

			result->rawSlideCount_ ++;
		}

		if(splashscreen->isStornoPressed())
			return;

		result->initDefaultSlideOrder();

		result_ = result;
	});

	return result_;
}

QString PowerpointPresentation::identification() const
{
	return identification_;
}

QPixmap PowerpointPresentation::icon() const
{
	static QPixmap icon_(":/icons/16/Microsoft PowerPoint_16px.png");
	return icon_;
}

int PowerpointPresentation::rawSlideCount() const
{
	return rawSlideCount_;
}

QString PowerpointPresentation::rawSlideIdentification(int i) const
{
	return tr("%1").arg(i+1);
}

QString PowerpointPresentation::rawSlideDescription(int i) const
{
	return slideTexts_[i];
}

PowerpointPresentation::PowerpointPresentation()
{

}
