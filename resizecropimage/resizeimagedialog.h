#include "resizecropschema.h"

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
#include <QIntValidator>
#include <QSignalMapper>
#include <QLabel>

class ResizeImageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResizeImageDialog(QWidget *parent = 0);
    ~ResizeImageDialog();
    QString getSourcePath();
    QString getTargetPath();
    int getTargetImageHeight();
    int getTargetImageWidth();
    //bool isReserveSourceProportion();
    int getPointCoordinateType();
    int getX();
    int getY();
    int getHeightAfterCut();
    int getWidthAfterCut();
    QString getTargetPipeMode();
    void setResizeCropArgs();

    ResizeCropArgs resizecropArgs;
    void getSourceProportion(QSize &sizeOfImage);
    QString inNormalStyleSheet = "QLineEdit{border:1px solid red }";
    QString NormalStyleSheet = "QLineEdit{border:1px solid gray border-radius:1px}";
private:
    // announce components in dialog window
    QLineEdit *sourceImagePathEdit; // raw Image dir input window
    QLineEdit *targetImagePathEdit; // target Image dir input window

    // bigger or smaller
    QLineEdit *targetHeightEdit; // target Image height
    QLineEdit *targetWidthEdit; // target Image width
    //SwitchButton *reserveHeightWidthButton;
    QCheckBox *KeepRatioCheckBox ;
    // cut
    QRadioButton *topLeftCoordinateButton, *centerCoordinateButton;
    QButtonGroup *poinCoordinateButtonGroup;
    QLineEdit *xEdit, *yEdit, *heightAfterCroppingEdit, *widthAfterCroppingEdit;

    // target pipe model
    QButtonGroup *pipeChoiceButtonGroup;
    QRadioButton *rgbButton, *grayButton;

    // final
    QLabel *warningWidget;
    QPushButton *resetButton;
    QPushButton *transformButton;
    QIntValidator *editSizeValidator, *editCoordinateValidator ;
    QCheckBox *cropCheckBox, *zoomCheckBox;
    QSize sourceSize;
    QHash <QString, bool> optionsReadyDict;
    QSignalMapper *selectPathSignalMapper;

private slots:
    void onCheckValidation();
    void onResetButtonClicked();
    void onTransferButtonClicked();
    void onSelectionPathButtonClicked(QString pathType);
    //void onEditIntegerArgLineEditChaneged();
    int checkReturnValidateIntValue(const QLineEdit *lineEdit, const QIntValidator *validator);
    bool onCheckCropOptions();
    bool onCheckZoomOptions();
    void onSourceProportionEditChaneged();
    int calcProportion(int value,  bool isHeight);
    void onTargetWidthEditCheck();
    void onTargetHeightEditCheck();
    void onxEditCheck();
    void onyEditCheck();
    void setEditCheckToolTip(QLineEdit *lineEdit, QIntValidator *validator);
    void onSourceEditCheck();
};

