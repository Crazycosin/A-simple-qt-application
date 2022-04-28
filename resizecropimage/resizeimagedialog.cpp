#include "resizeimagedialog.h"
#include "resizeprogressbar.h"
#include "utils.h"
#include <QtAlgorithms>
#include <QButtonGroup>
#include <QSignalMapper>
#include <QToolButton>
#include <QDir>
#include <QFileDialog>
#include <QWidgetAction>
#include <QImageReader>

ResizeImageDialog::ResizeImageDialog(QWidget *parent)
    : QDialog(parent)
{
    // signal
    this->selectPathSignalMapper = new QSignalMapper(this);
    this->editSizeValidator = new QIntValidator(0, 65536, this);
    this->editCoordinateValidator  = new QIntValidator(-1, 65536, this);

    // Images path
    QLabel *sourceImagePathLabel = new QLabel(tr("Source Image Path:"));
    this->sourceImagePathEdit = new QLineEdit;
    this->sourceImagePathEdit->setPlaceholderText(tr("Choose Image Directory"));
    connect(sourceImagePathEdit, &QLineEdit::textChanged, this, &ResizeImageDialog::onSourceEditCheck);
    QToolButton *sourceImagePathButton = new QToolButton;
    sourceImagePathButton->setText(tr("..."));
    sourceImagePathButton->setCursor(Qt::ArrowCursor);
    QWidgetAction *sourceImagePathAction = new QWidgetAction(sourceImagePathEdit);
    sourceImagePathAction->setDefaultWidget(sourceImagePathButton);
    sourceImagePathEdit->addAction(sourceImagePathAction, QLineEdit::TrailingPosition);
    connect(sourceImagePathButton, SIGNAL(clicked()), selectPathSignalMapper, SLOT(map()));
    selectPathSignalMapper->setMapping(sourceImagePathButton, "Source");

    QLabel *targetImagePathLabel = new QLabel(tr("Target Image Path:"));
    this->targetImagePathEdit = new QLineEdit;
    connect(targetImagePathEdit, &QLineEdit::textChanged, this, &ResizeImageDialog::onCheckValidation);
    this->targetImagePathEdit->setPlaceholderText(tr("Choose Image Directory"));
    QToolButton *targetImagePathButton = new QToolButton;
    targetImagePathButton->setText(tr("..."));
    targetImagePathButton->setCursor(Qt::ArrowCursor);
    QWidgetAction *targetImagePathAction = new QWidgetAction(targetImagePathEdit);
    targetImagePathAction->setDefaultWidget(targetImagePathButton);
    targetImagePathEdit->addAction(targetImagePathAction, QLineEdit::TrailingPosition);
    connect(targetImagePathButton, SIGNAL(clicked()), selectPathSignalMapper, SLOT(map()));
    selectPathSignalMapper->setMapping(targetImagePathButton, "Target");
    connect(selectPathSignalMapper, SIGNAL(mapped (QString)), this, SLOT(onSelectionPathButtonClicked(QString)));

    // zoom
    this->zoomCheckBox = new QCheckBox(tr("Zoom"));
    this->zoomCheckBox->setChecked(true);
    QLabel *targetImageHeightLabel = new QLabel(tr("Target Image Height (Pixel):"));
    QLabel *targetImageWidthLabel = new QLabel(tr("Target Image Width (Pixel):"));
    this->targetHeightEdit = new QLineEdit;
    connect(targetHeightEdit, &QLineEdit::textEdited, this, &ResizeImageDialog::onTargetHeightEditCheck);
    this->targetWidthEdit = new QLineEdit;
    connect(targetWidthEdit, &QLineEdit::textEdited, this, &ResizeImageDialog::onTargetWidthEditCheck);
    this->KeepRatioCheckBox  = new QCheckBox(tr("Reserve Source Proportion"));
    this->KeepRatioCheckBox ->setChecked(true);
    connect(KeepRatioCheckBox , &QCheckBox::toggled, this, &ResizeImageDialog::onSourceProportionEditChaneged);
    // cut out
    this->cropCheckBox = new QCheckBox(tr("Crop"));
    this->cropCheckBox->setChecked(true);
    QLabel *coordinateLocalLabel = new QLabel(tr("Point Coordinate:"));
    QLabel *xCoordinateLabel = new QLabel(tr("X:"));
    QLabel *yPostionLabel = new QLabel(tr("Y:"));
    QLabel *heightAfterCutLabel = new QLabel(tr("Height After Cropping:"));
    QLabel *widthAfterCutLabel = new QLabel(tr("Width After Cropping:"));
    this->xEdit = new QLineEdit;
    connect(xEdit, &QLineEdit::textChanged, this, &ResizeImageDialog::onxEditCheck);
    this->yEdit = new QLineEdit;
    connect(yEdit, &QLineEdit::textChanged, this, &ResizeImageDialog::onyEditCheck);
    this->heightAfterCroppingEdit = new QLineEdit;
    connect(heightAfterCroppingEdit, &QLineEdit::textChanged, this, &ResizeImageDialog::onyEditCheck);
    this->widthAfterCroppingEdit = new QLineEdit;
    connect(widthAfterCroppingEdit, &QLineEdit::textChanged, this, &ResizeImageDialog::onxEditCheck);
    this->topLeftCoordinateButton = new QRadioButton;
    this->topLeftCoordinateButton->setText(tr("Left Top (Default)"));
    this->topLeftCoordinateButton->setChecked(true);
    this->centerCoordinateButton = new QRadioButton;
    this->centerCoordinateButton->setText(tr("Center"));
    this->poinCoordinateButtonGroup = new QButtonGroup;
    this->poinCoordinateButtonGroup->addButton(this->topLeftCoordinateButton, 1);
    this->poinCoordinateButtonGroup->addButton(this->centerCoordinateButton, 2);

    // pipe choice
    QLabel *pipeChoiceLabel = new QLabel(tr("Target Image Channel Mode"));
    this->rgbButton = new QRadioButton;
    this->rgbButton->setText(tr("RGB (Default)"));
    this->rgbButton->setChecked(true);
    this->grayButton = new QRadioButton;
    this->grayButton->setText(tr("Gray"));
    this->pipeChoiceButtonGroup = new QButtonGroup;
    this->pipeChoiceButtonGroup->addButton(this->rgbButton, 1);
    //this->pipeChoiceButtonGroup->setId(this->rgbButton, 1);
    this->pipeChoiceButtonGroup->addButton(this->grayButton, 2);
   // this->pipeChoiceButtonGroup->setId(this->grayButton, 2);

    // final
    this->resetButton = new QPushButton(tr("Reset"));
    connect(resetButton, &QPushButton::clicked, this, &ResizeImageDialog::onResetButtonClicked);
    this->transformButton = new QPushButton(tr("Transform"));
    connect(transformButton, &QPushButton::clicked, this, &ResizeImageDialog::onTransferButtonClicked);

    // design layout
    QHBoxLayout *sourcePathLayout = new QHBoxLayout;
    sourcePathLayout->addWidget(sourceImagePathLabel);
    sourcePathLayout->addWidget(sourceImagePathEdit);

    QHBoxLayout *targetPathLayout = new QHBoxLayout;
    targetPathLayout->addWidget(targetImagePathLabel);
    targetPathLayout->addWidget(targetImagePathEdit);

    QHBoxLayout *targetImageHeightLayout = new QHBoxLayout;
    targetImageHeightLayout->addWidget(targetImageHeightLabel);
    targetImageHeightLayout->addWidget(targetHeightEdit);
    targetImageHeightLayout->addWidget(KeepRatioCheckBox );

    QHBoxLayout *targetImageWidthLayout = new QHBoxLayout;
    targetImageWidthLayout->addWidget(targetImageWidthLabel);
    targetImageWidthLayout->addWidget(targetWidthEdit);

    QHBoxLayout *pointCoordinateLayout = new QHBoxLayout;
    pointCoordinateLayout->addWidget(coordinateLocalLabel);
    pointCoordinateLayout->addWidget(topLeftCoordinateButton);
    pointCoordinateLayout->addWidget(centerCoordinateButton);

    QHBoxLayout *xyCoordinateLayout = new QHBoxLayout;
    xyCoordinateLayout->addWidget(xCoordinateLabel);
    xyCoordinateLayout->addWidget(xEdit);
    xyCoordinateLayout->addWidget(yPostionLabel);
    xyCoordinateLayout->addWidget(yEdit);

    QHBoxLayout *hwAfterCutLayout = new QHBoxLayout;
    hwAfterCutLayout->addWidget(heightAfterCutLabel);
    hwAfterCutLayout->addWidget(heightAfterCroppingEdit);
    hwAfterCutLayout->addWidget(widthAfterCutLabel);
    hwAfterCutLayout->addWidget(widthAfterCroppingEdit);

    QHBoxLayout *targetPipeChoiceLayout = new QHBoxLayout;
    targetPipeChoiceLayout->addWidget(rgbButton);
    targetPipeChoiceLayout->addWidget(grayButton);

    this->warningWidget = new QLabel(this);
    QHBoxLayout *resetOrTransLayout = new QHBoxLayout;
    resetOrTransLayout->addWidget(warningWidget);
    resetOrTransLayout->addWidget(resetButton);
    resetOrTransLayout->addWidget(transformButton);

    QVBoxLayout *finalLayout = new QVBoxLayout;
    finalLayout->addLayout(resetOrTransLayout);

    QVBoxLayout *ImagePathLayout = new QVBoxLayout;
    ImagePathLayout->addLayout(sourcePathLayout);
    ImagePathLayout->addLayout(targetPathLayout);

    QVBoxLayout *zoomLayout = new QVBoxLayout;
    zoomLayout->addWidget(zoomCheckBox);
    QVBoxLayout *zoomOptionsLayout = new QVBoxLayout;
    QWidget *zoomWidget = new QWidget;
    zoomOptionsLayout->addLayout(targetImageHeightLayout);
    zoomOptionsLayout->addLayout(targetImageWidthLayout);
    zoomWidget->setLayout(zoomOptionsLayout);
    zoomLayout->addWidget(zoomWidget);

    QVBoxLayout *cutOutLayout = new QVBoxLayout;
    QVBoxLayout *cropOptionsLayout = new QVBoxLayout;
    QWidget *cropWidget = new QWidget;
    cutOutLayout->addWidget(cropCheckBox);
    cropOptionsLayout->addLayout(pointCoordinateLayout);
    cropOptionsLayout->addLayout(xyCoordinateLayout);
    cropOptionsLayout->addLayout(hwAfterCutLayout);
    cropWidget->setLayout(cropOptionsLayout);
    cutOutLayout->addWidget(cropWidget);
    connect(cropCheckBox, &QCheckBox::toggled, this, [=](bool checked){
        if (checked)
            cropWidget->setVisible(true);
        else
            cropWidget->setVisible(false);
        //onCheckcropOptions();
        onCheckValidation();
    });

    QVBoxLayout *targetPipeLayout = new QVBoxLayout;
    targetPipeLayout->addWidget(pipeChoiceLabel);
    targetPipeLayout->addLayout(targetPipeChoiceLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(ImagePathLayout);
    mainLayout->addLayout(zoomLayout);
    mainLayout->addLayout(cutOutLayout);
    mainLayout->addLayout(targetPipeLayout);
    mainLayout->addLayout(finalLayout);
    this->setLayout(mainLayout);
    // window title
    this->setWindowTitle(QString(tr("Resize&Crop")));
    onCheckValidation();
    connect(zoomCheckBox, &QCheckBox::toggled, this, [=](bool checked){
        if (checked)
            zoomWidget->setVisible(true);
        else
            zoomWidget->setVisible(false);
        //onCheckZoomOptions();
        onCheckValidation();
    });
    // first no show warning
    warningWidget->setText("");
}

ResizeImageDialog::~ResizeImageDialog(){
    delete editSizeValidator;
    delete editCoordinateValidator;
    //selectPathSignalMapper = nullptr;
    delete selectPathSignalMapper;
}

void ResizeImageDialog::onxEditCheck(){
    int width = 0;
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(sender());
    if (this->zoomCheckBox->isChecked()){
        width = getTargetImageWidth();
    }
    else{
        QSize sizeOfImage;
        getSourceProportion(sizeOfImage);
        width = sizeOfImage.width();
    }
    int cutWidth = 0;
    if (getPointCoordinateType() == 1){
        // left top
        cutWidth = getWidthAfterCut();
    }
    else{
        cutWidth = getWidthAfterCut() / 2;
    }
    int x = getX();
    if ((x + cutWidth) > width) {
        int maxValue = width - (x + cutWidth - lineEdit->text().toInt());
        QIntValidator *validator = new QIntValidator(0, maxValue, this);
        setEditCheckToolTip(lineEdit, validator);
        warningWidget->setText(
                    QString(
                        tr("x + cutWidth <= real width[%1]!"))
                    .arg(width));
        this->transformButton->setDisabled(true);
    }
    else{
        setEditCheckToolTip(lineEdit, this->editSizeValidator);
        onCheckValidation();
    }
}

void ResizeImageDialog::onyEditCheck(){
    int height = 0;
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(sender());
    if (this->zoomCheckBox->isChecked()){
        height = getTargetImageHeight();
    }
    else{
        QSize sizeOfImage;
        getSourceProportion(sizeOfImage);
        height = sizeOfImage.height();
    }
    int cutHeight = 0;
    if (getPointCoordinateType() == 1){
        // left top
        cutHeight = getHeightAfterCut();

    }
    else{
        cutHeight = getHeightAfterCut() / 2;
    }
    int y = getY();
    if ((y + cutHeight) > height) {
        int maxValue = height - ( y + cutHeight - lineEdit->text().toInt());
        QIntValidator *validator = new QIntValidator(0, maxValue, this);
        setEditCheckToolTip(lineEdit, validator);
        warningWidget->setText(
                    QString(
                        tr("y + cutHeight  <= real height[%1]!"))
                    .arg(height));
        this->transformButton->setDisabled(true);
    }
    else{
        setEditCheckToolTip(lineEdit, this->editSizeValidator);
        onCheckValidation();
    }
}

void ResizeImageDialog::onTargetHeightEditCheck(){
    if (this->KeepRatioCheckBox ->isChecked()){
        if(checkReturnValidateIntValue(this->targetHeightEdit, this->editSizeValidator)){
            int height = getTargetImageHeight();
            int width = calcProportion(height, true);
            this->targetWidthEdit->setText(QString("%1").arg(width));
        }
        else{
            this->targetWidthEdit->setText("");
        }
    }
    onxEditCheck();
    onyEditCheck();
}

void ResizeImageDialog::onTargetWidthEditCheck(){
    if (this->KeepRatioCheckBox ->isChecked()){
        if(checkReturnValidateIntValue(this->targetWidthEdit, this->editSizeValidator)){
            int width = getTargetImageWidth();
            int height = calcProportion(width, false);
            this->targetHeightEdit->setText(QString("%1").arg(height));
        }
        else{
            this->targetHeightEdit->setText("");
        }
    }
    onxEditCheck();
    onyEditCheck();
}

void ResizeImageDialog::onSourceEditCheck(){
    QDir sourceDir(getSourcePath());
    if ((!sourceDir.exists()) || (getSourcePath().isEmpty())) {
        qDebug() << "Source Image path:" << getSourcePath() << "\n";
        warningWidget->setText(tr("Source Image path not exist!"));
        this->transformButton->setDisabled(true);
        return;
    }
    QList<QFileInfo> sourceFileInfo;
    QSet<QByteArray> validFormat = QImageReader::supportedImageFormats().toSet();
    recursiveAllFiles(getSourcePath(), sourceFileInfo, &validFormat);
    sourceImagePathEdit->setToolTip(QString("%1 images").
                                       arg(sourceFileInfo.count()));
    bool isExistFile = false;
    foreach(QFileInfo fileInfo, sourceFileInfo)
    {
        QImageReader reader(fileInfo.absoluteFilePath());
        if (reader.canRead()){
            isExistFile = true;
            this->sourceSize = reader.size();
            break;
        }
    }
    if (!isExistFile){
        sourceImagePathEdit->setToolTip(QString("%1 images, but no file can read!").
                                           arg(sourceFileInfo.count()));
        sourceImagePathEdit->setStyleSheet(ResizeImageDialog::inNormalStyleSheet);
        warningWidget->setText(tr("There is no valid image file in Source Image path!"));
        this->transformButton->setDisabled(true);
        optionsReadyDict.insert("sourcePath", false);
        return;
    }
    sourceImagePathEdit->setStyleSheet(ResizeImageDialog::NormalStyleSheet);
    optionsReadyDict.insert("sourcePath", true);
}

void ResizeImageDialog::onCheckValidation(){
    if (!optionsReadyDict.value("sourcePath", false)){
        warningWidget->setText(tr("Source Image path is not ready!"));
        this->transformButton->setDisabled(true);
        return;
    }
    QDir targetDir(getTargetPath());
    if ((!targetDir.exists()) || (getTargetPath().isEmpty())) {
        warningWidget->setText(tr("Target Image not exist!"));
        this->transformButton->setDisabled(true);
        return;
    }
    if (!onCheckZoomOptions())
        return;
    if (!onCheckCropOptions())
        return;
    warningWidget->setText(QString());
    transformButton->setEnabled(true);
}

bool ResizeImageDialog::onCheckCropOptions(){
    if (this->cropCheckBox->isChecked()){
        if (getX() < 0){
            warningWidget->setText(tr("X Point is an Invaild Value!"));
            this->transformButton->setDisabled(true);
            return false;
        };
        if (getY() < 0){
            warningWidget->setText(tr("Y Point is an Invaild Value!"));
            this->transformButton->setDisabled(true);
            return false;
        }
        if (!getHeightAfterCut()){
            warningWidget->setText(tr("Height After Cut is an Invaild Value!"));
            this->transformButton->setDisabled(true);
            return false;
        }
        if (!getWidthAfterCut()){
            warningWidget->setText(tr("Width After Cut is an Invaild Value!"));
            this->transformButton->setDisabled(true);
            return false;
        }
    }
    return true;
}

bool ResizeImageDialog::onCheckZoomOptions(){
    if (this->zoomCheckBox->isChecked()){
        if (!getTargetImageHeight()){
            warningWidget->setText(tr("Target Image Height is an Invaild Value!"));
            this->transformButton->setDisabled(true);
            return false;
        };
        if (!getTargetImageWidth()){
            warningWidget->setText(tr("Target Image Width is an Invaild Value!"));
            this->transformButton->setDisabled(true);
            return false;
        }
    }
    return true;
}

void ResizeImageDialog::onResetButtonClicked(){
    // reset options
    this->sourceImagePathEdit-> clear();
    this->targetImagePathEdit->clear();

    this->targetHeightEdit->clear();
    this->targetWidthEdit->clear();
    this->KeepRatioCheckBox ->setChecked(true);
    this->cropCheckBox->setChecked(true);
    this->zoomCheckBox->setChecked(true);
    this->topLeftCoordinateButton->setAutoExclusive(false);
    this->topLeftCoordinateButton->setChecked(true);
    this->topLeftCoordinateButton->setAutoExclusive(true);

    this->centerCoordinateButton->setAutoExclusive(false);
    this->centerCoordinateButton->setChecked(false);
    this->centerCoordinateButton->setAutoExclusive(true);

    this->xEdit->clear();
    this->yEdit->clear();
    this->heightAfterCroppingEdit->clear();
    this->widthAfterCroppingEdit->clear();

    this->rgbButton->setAutoExclusive(false);
    this->rgbButton->setChecked(true);
    this->rgbButton->setAutoExclusive(true);

    this->grayButton->setAutoExclusive(false);
    this->grayButton->setChecked(false);
    this->grayButton->setAutoExclusive(true);

    this->transformButton->setDisabled(true);

    onCheckValidation();
}

void ResizeImageDialog::onTransferButtonClicked(){
    setResizeCropArgs();
    ResizeCropArgs *args = &(this->resizecropArgs);
    ResizeProgressBarDialog *resizeProgressBarDialog = new ResizeProgressBarDialog(
                0,
                args);
    if (resizeProgressBarDialog->exec()){}
    // delete and set NULL to dialog to avoid wild pointer
    delete resizeProgressBarDialog;
    resizeProgressBarDialog = NULL;
}

void ResizeImageDialog::onSelectionPathButtonClicked(QString pathType)
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Image Directory"),
                                                    QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly
                                                    |QFileDialog::DontResolveSymlinks
                                                    );
    if (0 != &dir && "" != dir) {
        if (QString::compare(pathType, "Target") == 0)
            targetImagePathEdit->setText(dir);
        else{
            sourceImagePathEdit->setText(dir);
        }
    }
    onCheckValidation();
}

void ResizeImageDialog::setEditCheckToolTip(
        QLineEdit *lineEdit,
        QIntValidator *validator
        ){
    QString text = lineEdit->text();
    int pos = 0;
    if(validator->validate(text, pos) != QIntValidator::Acceptable)
    {
       // 错误提示
       lineEdit->setToolTip(QString("range for(%1-%2)")
                            .arg(validator->bottom())
                            .arg(validator->top()));
       // 设置LineEdit变为红色
       lineEdit->setStyleSheet(ResizeImageDialog::inNormalStyleSheet);
       this->transformButton->setDisabled(true);
    }
    else
    {
        // LineEdit恢复之前的状态
       lineEdit->setStyleSheet(ResizeImageDialog::NormalStyleSheet);
       onCheckValidation();
    }
}

QString ResizeImageDialog::getSourcePath(){
    return this->sourceImagePathEdit->text();

}

QString ResizeImageDialog::getTargetPath(){
    return this->targetImagePathEdit->text();
}

int ResizeImageDialog::getTargetImageHeight(){
    return checkReturnValidateIntValue(this->targetHeightEdit, this->editSizeValidator);
}

int ResizeImageDialog::getTargetImageWidth(){
     return checkReturnValidateIntValue(this->targetWidthEdit, this->editSizeValidator);
}

int ResizeImageDialog::checkReturnValidateIntValue(
        const QLineEdit *lineEdit,
        const QIntValidator *validator
        ){
    QString text = lineEdit->text();
    int pos = 0;
    if (validator->validate(text, pos) != QIntValidator::Acceptable){
        return 0;
    }
    return text.toInt();
}

int ResizeImageDialog::getPointCoordinateType(){
    return  this->poinCoordinateButtonGroup->checkedId();
}

int ResizeImageDialog::getX(){
    return checkReturnValidateIntValue(this->xEdit, this->editCoordinateValidator);
}

int ResizeImageDialog::getY(){
    return checkReturnValidateIntValue(this->yEdit, this->editCoordinateValidator);
}

int ResizeImageDialog::getHeightAfterCut(){
    return checkReturnValidateIntValue(this->heightAfterCroppingEdit, this->editSizeValidator);
}

int ResizeImageDialog::getWidthAfterCut(){
    return checkReturnValidateIntValue(this->widthAfterCroppingEdit, this->editSizeValidator);
}

QString ResizeImageDialog::getTargetPipeMode(){
    if (this->pipeChoiceButtonGroup->checkedId() == 1)
        return "RGB";
     return  "Gray";
}

void ResizeImageDialog::setResizeCropArgs(){
    this->resizecropArgs.isCrop = this->cropCheckBox->isChecked();
    this->resizecropArgs.sourcePath = getSourcePath();
    this->resizecropArgs.targetPath = getTargetPath();
    this->resizecropArgs.isResize = this->zoomCheckBox->isChecked();
    this->resizecropArgs.resizeHeight = getTargetImageHeight();
    this->resizecropArgs.resizeWidth = getTargetImageWidth();
    int x = getX();
    int y = getY();
    if (getPointCoordinateType() != 1){
        x = x - getWidthAfterCut()/ 2;
        y = y - getHeightAfterCut() / 2;
     }
    this->resizecropArgs.cropRect = QRect(x, y, getWidthAfterCut(), getHeightAfterCut());
    this->resizecropArgs.pipeMode = getTargetPipeMode();
}

void ResizeImageDialog::getSourceProportion(QSize &sizeOfImage){
    sizeOfImage = this->sourceSize;
}

int ResizeImageDialog::calcProportion(
        int value,  bool isHeight){
    // auto modify height, width
    QSize sizeOfImage;
    getSourceProportion(sizeOfImage);
    int height = sizeOfImage.height();
    int width = sizeOfImage.width();
    if (isHeight){
        return value * width / height;
    }
    return value * height / width;
}

void ResizeImageDialog::onSourceProportionEditChaneged(){
    if (this->KeepRatioCheckBox ->isChecked()){
        if (getSourcePath().isEmpty()){
            warningWidget->setText(tr(" Source Path is empty, reserve proportion has been no effect!"));
            return;
        }
        bool isCalc = false;
        if (getTargetImageHeight()){
            int targetWidth = calcProportion(getTargetImageHeight(), true);
            this->targetWidthEdit->setText(QString("%1").arg(targetWidth));
            isCalc = true;
        }
        if (getTargetImageWidth() && (!isCalc)) {
            isCalc = true;
            int targetHeight = calcProportion(getTargetImageWidth(), false);
            this->targetWidthEdit->setText(QString("%1").arg(targetHeight));
        }
    }
}
