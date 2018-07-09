#include "presentation_powerpoint.h"

#include <QAxObject>
#include <QFile>
#include <QDir>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QApplication>
#include <QDesktopWidget>

#include "gui/splashscreen.h"
#include "presentation/powerpoint/presentationpropertieswidget_powerpoint.h"
#include "gui/settingsdialog.h"
#include "job/activexjobthread.h"
#include "presentation/powerpoint/presentationengine_powerpoint.h"
#include "util/standarddialogs.h"
#include "util/scopeexit.h"

#include "gui/activexdebugdialog.h"

const QStringList Presentation_PowerPoint::validExtensions {"ppt", "pptx"};

bool Presentation_PowerPoint::isPowerpointFile(const QFileInfo &file)
{
	return validExtensions.contains(file.suffix());
}

QSharedPointer<Presentation_PowerPoint> Presentation_PowerPoint::create(const QString &filename)
{
	QSharedPointer<Presentation_PowerPoint> result_;

	splashscreen->asyncAction(tr("Načítání \"%1\"").arg(QFileInfo(filename).fileName()), true, *activeXJobThread, [&]{
		QSharedPointer<Presentation_PowerPoint> result(new Presentation_PowerPoint());

		result->filePath_ = filename;
		result->identification_ = QFileInfo(filename).completeBaseName();

		QAxObject *axApplication = activeXJobThread->axPowerPointApplication;

		if(!axApplication)
			return standardErrorDialog(tr("Program nedetekoval instalaci PowerPointu. Bez nainstalového PowerPointu nelze pracovat s powerpointovými prezentacemi."));

		if(splashscreen->isStornoPressed())
			return;

		auto axPresentations = axApplication->querySubObject("Presentations");
		auto axPresentation = axPresentations->querySubObject(
					"Open(QString,Office::MsoTriState,Office::MsoTriState,Office::MsoTriState)",
					QDir::toNativeSeparators(filename), true, false, false
					);

		if(!axPresentation)
			return standardErrorDialog(tr("Nepodařilo se načíst prezentaci \"%1\".").arg(filename));

		SCOPE_EXIT({
			axPresentation->dynamicCall("Close()");
			delete axPresentation;
		});

		if(splashscreen->isStornoPressed())
			return;

		auto axSlides = axPresentation->querySubObject("Slides");
		const int slideCount = axSlides->property("Count").toInt();

		const QDir tmpDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation));

		for(int slideI = 1; slideI <= slideCount; slideI++) {
			if(splashscreen->isStornoPressed())
				return;

			splashscreen->setProgress(slideI, slideCount);

			auto axSlide = axSlides->querySubObject("Item(QVariant)", slideI);
			auto axTransition = axSlide->querySubObject("SlideShowTransition");

			if(axTransition->property("Hidden").toInt() == (int) Office::MsoTriState::msoTrue)
				continue;

			QSharedPointer<Slide> slide(new Slide());
			result->slides_.append(slide);
			slide->ppIndex = slideI;

			// Obtain slide text
			{
				auto &slideText = slide->text;

				auto axShapes = axSlide->querySubObject("Shapes");
				int shapeCount = axShapes->property("Count").toInt();

				for(int shapeI = 1; shapeI <= shapeCount; shapeI++) {
					auto axShape = axShapes->querySubObject("Item(QVariant)", shapeI);

					if(axShape->property("HasTextFrame").toInt() == (int) Office::MsoTriState::msoFalse)
						continue;

					auto axTextFrame = axShape->querySubObject("TextFrame");
					auto axTextRange = axTextFrame->querySubObject("TextRange");

					slideText.append(' ');
					slideText.append(axTextRange->property("Text").toString());
				}

				slideText.replace(QRegularExpression("\\s+"), " ");

				if(slideText.length() > maxDescriptionLength) {
					slideText.resize(maxDescriptionLength - 3);
					slideText.append("...");
				}
			}

			// Obtain slide image
			/*do {
				const QString filename = QDir::toNativeSeparators(tmpDir.absoluteFilePath("strawLumenThumbTmp.png"));

				axSlide->dynamicCall("Export(QString,QString,Long,Long)", filename, "PNG", 512, 512).toBool();
				slide->thumbnail = QPixmap(filename);
				QFile(filename).remove();

			} while(false);*/

			result->slideCount_ ++;
		}

		if(splashscreen->isStornoPressed())
			return;

		result->weakPtr_ = result;

		result_ = result;
	});

	return result_;
}

Presentation_PowerPoint::~Presentation_PowerPoint()
{
	// Prevent destroying the engine before deactivatePresentation is possibly called on the activeX thread
	activeXJobThread->waitJobsDone();
}

QString Presentation_PowerPoint::identification() const
{
	return identification_;
}

QPixmap Presentation_PowerPoint::icon() const
{
	static QPixmap icon(":/icons/16/Microsoft PowerPoint_16px.png");
	return icon;
}

QPixmap Presentation_PowerPoint::specialIcon() const
{
	static QPixmap autoSlidePixmap(":/icons/16/Repeat_16px.png");
	return isAutoPresentation_ ? autoSlidePixmap : QPixmap();
}

QWidget *Presentation_PowerPoint::createPropertiesWidget(QWidget *parent)
{
	return new PresentationPropertiesWidget_PowerPoint(weakPtr_, parent);
}

int Presentation_PowerPoint::slideCount() const
{
	return isAutoPresentation_ ? 1 : slideCount_;
}

QString Presentation_PowerPoint::slideIdentification(int i) const
{
	return isAutoPresentation_ ? "" : tr("%1").arg(i+1);
}

QPixmap Presentation_PowerPoint::slideIdentificationIcon(int ) const
{
	static QPixmap autoSlidePixmap(":/icons/16/Repeat_16px.png");
	return isAutoPresentation_ ? autoSlidePixmap : QPixmap();
}

QString Presentation_PowerPoint::slideDescription(int i) const
{
	return isAutoPresentation_ ? QString() : slides_[i]->text;
}

PresentationEngine *Presentation_PowerPoint::engine() const
{
	return presentationEngine_PowerPoint;
}

void Presentation_PowerPoint::activatePresentation(int startingSlide)
{
	QSharedPointer<Presentation_PowerPoint> selfPtr(weakPtr_);

	int startingSlide_ = slides_[startingSlide]->ppIndex;
	QRect rect = settingsDialog->settings().projectionDisplayGeometry();

	//splashscreen->asyncAction(tr("Spouštění prezentace"), false, *activeXJobThread, [this, selfPtr, &result]{
	activeXJobThread->executeNonblocking([this, selfPtr, startingSlide_, rect]{
		auto &pe = *presentationEngine_PowerPoint;

		pe.axPresentation_ = pe.axPresentations_->querySubObject("Open(QString,Office::MsoTriState,Office::MsoTriState,Office::MsoTriState)", QDir::toNativeSeparators(filePath_), true, false, false);
		if(!pe.axPresentation_)
			return standardErrorDialog(tr("Nepodařilo se spustit prezentaci \"%1\".").arg(filePath_));

		pe.axSlides_ = pe.axPresentation_->querySubObject("Slides");

		pe.axSSSettings_ = pe.axPresentation_->querySubObject("SlideShowSettings");
		pe.axSSSettings_->dynamicCall("SetShowType(Office::PpSlideShowType)", (int) Office::PowerPoint::PpSlideShowType::ppShowTypeKiosk);

		/*pe.axSSSettings_->dynamicCall("SetRangeType(int)", (int) Office::PowerPoint::PpSlideShowRangeType::ppShowSlideRange);
		pe.axSSSettings_->dynamicCall("SetStartingSlide(int)", startingSlide_);
		pe.axSSSettings_->dynamicCall("SetEndingSlide(int)", startingSlide_+1); Does not work like expected*/

		pe.axSSSettings_->dynamicCall(
					"SetAdvanceMode(Office::PpSlideShowAdvanceMode )",
					(isAutoPresentation_ ? (int) Office::PowerPoint::PpSlideShowAdvanceMode::ppSlideShowUseSlideTimings : (int) Office::PowerPoint::PpSlideShowAdvanceMode::ppSlideShowManualAdvance)
					);

		pe.axSSSettings_->dynamicCall("Run()");

		pe.axPresentationWindow_ = pe.axPresentation_->querySubObject("SlideShowWindow");
		pe.axSSView_ = pe.axPresentationWindow_->querySubObject("View");

		if(!isAutoPresentation_)
			pe.axSSView_->dynamicCall("GotoSlide(int)", startingSlide_);

		presentationEngine_PowerPoint->setDisplay(rect);
	});
}

void Presentation_PowerPoint::deactivatePresentation()
{
	QSharedPointer<Presentation_PowerPoint> selfPtr(weakPtr_);
	activeXJobThread->executeNonblocking([this, selfPtr]{
		auto &pe = *presentationEngine_PowerPoint;

		pe.axPresentation_->dynamicCall("Close()");
		delete pe.axPresentation_;

		pe.axPresentation_ = nullptr;
	});
}

void Presentation_PowerPoint::setSlide(int localSlideId)
{
	QSharedPointer<Presentation_PowerPoint> selfPtr(weakPtr_);
	activeXJobThread->executeNonblocking([this, selfPtr, localSlideId]{
		auto &pe = *presentationEngine_PowerPoint;

		if(!pe.axPresentation_ || isAutoPresentation_)
			return;

		pe.axSSView_->dynamicCall("GotoSlide(int)", slides_[localSlideId]->ppIndex);
	});
}

Presentation_PowerPoint::Presentation_PowerPoint()
{

}
