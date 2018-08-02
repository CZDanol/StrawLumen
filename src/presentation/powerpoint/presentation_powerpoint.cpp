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
#include "job/settings.h"
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

QSharedPointer<Presentation_PowerPoint> Presentation_PowerPoint::createFromFilename(const QString &filename)
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
		result->blackSlideBefore_ = settings->setting_ppt_blackSlideBefore();
		result->blackSlideAfter_ = settings->setting_ppt_blackSlideAfter();

		result_ = result;
	});

	return result_;
}

QSharedPointer<Presentation_PowerPoint> Presentation_PowerPoint::createFromJSON(const QJsonObject &json)
{
	QSharedPointer<Presentation_PowerPoint> result = createFromFilename(json["filePath"].toString());
	if(!result)
		return nullptr;

	result->isAutoPresentation_ = json["isAutoPresentation"].toBool();
	result->blackSlideBefore_ = json["blackSlideBefore"].toBool();
	result->blackSlideAfter_ = json["blackSlideAfter"].toBool();

	return result;
}

QJsonObject Presentation_PowerPoint::toJSON() const
{
	return QJsonObject {
		{"filePath", filePath_},
		{"isAutoPresentation", isAutoPresentation_},
		{"blackSlideBefore", blackSlideBefore_},
		{"blackSlideAfter", blackSlideAfter_}
	};
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
	if(isAutoPresentation_)
		return 1;

	int result = slideCount_;

	if(blackSlideBefore_)
		result += 1;

	if(blackSlideAfter_)
		result += 1;

	return result;
}

QString Presentation_PowerPoint::slideIdentification(int i) const
{
	if(isAutoPresentation_)
		return QString();

	if(blackSlideBefore_ && i == 0)
		return QString();

	if(blackSlideBefore_)
		i -= 1;

	if(blackSlideAfter_ && i == slides_.count())
		return QString();

	return tr("%1").arg(i+1);
}

QPixmap Presentation_PowerPoint::slideIdentificationIcon(int i) const
{
	static QPixmap autoSlidePixmap(":/icons/16/Repeat_16px.png");
	static QPixmap blackScreenBeforePixmap(":/icons/16/Sign Out_16px.png");
	static QPixmap blackScreenAfterPixmap(":/icons/16/Logout Rounded Left_16px.png");

	if(isAutoPresentation_)
		return autoSlidePixmap;

	if(blackSlideBefore_ && i == 0)
		return blackScreenBeforePixmap;

	if(blackSlideBefore_)
		i -= 1;

	if(blackSlideAfter_ && i == slides_.size())
		return blackScreenAfterPixmap;

	return QPixmap();
}

QString Presentation_PowerPoint::slideDescription(int i) const
{
	if(isAutoPresentation_)
		return QString();

	if(blackSlideBefore_ && i == 0)
		return QString();

	if(blackSlideBefore_)
		i -= 1;

	if(blackSlideAfter_ && i == slides_.count())
		return QString();

	return slides_[i]->text;
}

PresentationEngine *Presentation_PowerPoint::engine() const
{
	return presentationEngine_PowerPoint;
}

QString Presentation_PowerPoint::classIdentifier() const
{
	return "powerPoint.powerPoint";
}
#include <QThread>
void Presentation_PowerPoint::activatePresentation(int startingSlide)
{
	QSharedPointer<Presentation_PowerPoint> selfPtr(weakPtr_);

	const QRect rect = settings->projectionDisplayGeometry();
	const int pptSlideI = getPptSlideI(startingSlide);

	//splashscreen->asyncAction(tr("Spouštění prezentace"), false, *activeXJobThread, [this, selfPtr, &result]{
	activeXJobThread->executeNonblocking([this, selfPtr, startingSlide, rect, pptSlideI]{
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

		setSlide_axThread(pptSlideI);
		presentationEngine_PowerPoint->setDisplay_axThread(rect);
	});
}

void Presentation_PowerPoint::deactivatePresentation()
{
	QSharedPointer<Presentation_PowerPoint> selfPtr(weakPtr_);
	activeXJobThread->executeNonblocking([this, selfPtr]{
		auto &pe = *presentationEngine_PowerPoint;

		if(!pe.axPresentation_) {
			pe.axPresentation_ = nullptr;
			return;
		}

		pe.axPresentation_->dynamicCall("Close()");
		delete pe.axPresentation_;

		pe.axPresentation_ = nullptr;
	});
}

void Presentation_PowerPoint::setSlide(int localSlideId, bool force)
{
	Q_UNUSED(force);

	QSharedPointer<Presentation_PowerPoint> selfPtr(weakPtr_);

	const int pptSlideI = getPptSlideI(localSlideId);

	activeXJobThread->executeNonblocking([this, selfPtr, pptSlideI]{
		auto &pe = *presentationEngine_PowerPoint;

		if(!pe.axPresentation_)
			return;

		setSlide_axThread(pptSlideI);
	});
}

bool Presentation_PowerPoint::isSlideBlackScreen(int localSlideId) const
{
	return getPptSlideI(localSlideId) == -1;
}

Presentation_PowerPoint::Presentation_PowerPoint()
{

}

void Presentation_PowerPoint::setSlide_axThread(int pptSlideI)
{
	auto &pe = *presentationEngine_PowerPoint;

	if(pptSlideI == -2)
		return;

	if(pptSlideI == -1) {
		pe.axSSView_->dynamicCall("SetState(int)", (int) Office::PowerPoint::PpSlideShowState::ppSlideShowBlackScreen);
		return;
	}

	pe.axSSView_->dynamicCall("SetState(int)", (int) Office::PowerPoint::PpSlideShowState::ppSlideShowRunning);
	pe.axSSView_->dynamicCall("GotoSlide(int)", pptSlideI);
}

int Presentation_PowerPoint::getPptSlideI(int localSlideId) const
{
	if(isAutoPresentation_)
		return -2;

	if(blackSlideBefore_ && localSlideId == 0)
		return -1;

	if(blackSlideBefore_)
		localSlideId -= 1;

	if(blackSlideAfter_ && localSlideId == slides_.count())
		return -1;

	return slides_[localSlideId]->ppIndex;
}
