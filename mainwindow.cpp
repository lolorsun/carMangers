#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //开始就显示车辆管理
//    on_actionCalc_triggered();
    on_actionCar_triggered();
    connectDb();//连接数据库
    initData();//初始化数据
}

MainWindow::~MainWindow()
{
    delete ui;
}
//车辆管理菜单
void MainWindow::on_actionCar_triggered()
{
   //切换到车辆管理界面
    ui->stackedWidget->setCurrentWidget(ui->car);
    //设置切换界面时，标签的字体也相应发生变化
    ui->label->setText("车辆管理");
}
//销售统计菜单
void MainWindow::on_actionCalc_triggered()
{
    //切换到销售统计界面
    ui->stackedWidget->setCurrentWidget(ui->calc);
    //设置切换界面时，标签的字体也相应发生变化
    ui->label->setText("销售统计");
}
//数据库连接
void MainWindow::connectDb()
{
    //添加数据库
   /* QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setUserName("lolors");
    db.setPassword("mysql054636");
    db.setDatabaseName("car");
    */
    //方案更改，数据库更换为sqlite3数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/home/linux/lolors/car");
    //连接数据库
    if(db.open()==false)
    {
        QMessageBox::warning(this,"警告","数据库连接失败",db.lastError().text());
                return;
    }

}
//初始化数据
void MainWindow::initData()
{
    QSqlQueryModel *queryModel = new QSqlQueryModel(this);//建立数据库模型
    queryModel->setQuery("select name from factory");//模型赋值
//      queryModel->setQuery("select sum from brand");

    ui->comboBoxFactory->setModel(queryModel);//下拉框设置模型
}

//选择厂商对应所有的车系
void MainWindow::on_comboBoxFactory_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "请选择厂家")
    {
        ui->comboBoxBrand->clear();//品牌
        ui->lineEditPrice->clear();//报价
        ui->lineEditTotal->clear();//金额
        ui->spinBox->setValue(0);//数量选择框
        ui->labelLast->setText("0");//剩余显示
        ui->spinBox->setEnabled(false);//设置选择框初始时失能
        ui->lineEditTotal->setEnabled(false);
        ui->ButtonSure->setEnabled(false);
    }
    else
    {
        ui->comboBoxBrand->clear();//清空
        QSqlQuery query;
        QString sql = QString("select name from brand where factory = '%1'").arg(arg1);//封包
        query.exec(sql);//执行sql语句
        while(query.next())//获取内容
        {
            QString name = query.value(0).toString();
            ui->comboBoxBrand->addItem(name);//得先清空
        }
        ui->spinBox->setEnabled(true);//设置选择框使能
    }
}
//选择车系，查看报价和数量
void MainWindow::on_comboBoxBrand_currentIndexChanged(const QString &arg1)
{
    QSqlQuery query;
    QString sql = QString("select price,last from brand where factory = '%1' and name ='%2'").arg(ui->comboBoxFactory->currentText())
            .arg(arg1);//封包
    query.exec(sql);//执行sql语句
    while(query.next())//获取内容
    {
        int price = query.value("price").toInt();
        ui->lineEditPrice->setText(QString::number(price));
        int last = query.value("last").toInt();
        ui->labelLast->setText(QString::number(last));

    }
}

//选择框处理

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    //厂家
    QString factoryStr = ui->comboBoxFactory->currentText();
    //品牌
    QString brandStr = ui->comboBoxBrand->currentText();

    //设置确认按钮使能还是失能
    if(arg1 == 0)
    {
        ui->ButtonSure->setEnabled(false);
    }
    else
    {
        ui->ButtonSure->setEnabled(true);
    }

    QSqlQuery query;
    QString sql = QString("select price,last from brand where factory = '%1' and name ='%2'").arg(factoryStr)
            .arg(brandStr);//封包
    query.exec(sql);//执行sql语句
    int last;
    int price;
    while(query.next())//获取内容
    {
//        int last = query.value("last").toInt();不能在里面定义，last变量

         last = query.value("last").toInt();
         price = query.value("price").toInt();
    }

    //判断选择框的值是否超过数据库剩余数量
    if(last<arg1)
    {
        ui->spinBox->setValue(0);
        return;
    }
    //获取数据库剩余数量
//int tempNum = ui->labelLast->text().toInt()-arg1;
    int tempNum = last-arg1;
    ui->labelLast->setText(QString::number(tempNum));
//总价
    int total = price*arg1;
    ui->lineEditTotal->setText(QString::number(total));

}
//取消按钮
void MainWindow::on_ButtonCancle_clicked()
{
    //on_comboBoxFactory_currentIndexChanged("请选择厂家");
    ui->comboBoxFactory->setCurrentIndex(0);
}

void MainWindow::on_ButtonSure_clicked()
{
    //厂家
    QString factoryStr = ui->comboBoxFactory->currentText();
    //品牌
    QString brandStr = ui->comboBoxBrand->currentText();
    //剩余标签里的值
    QString last = ui->labelLast->text();
    qDebug()<< last;
    int lastLab = last.toInt();
    QSqlQuery query;
    QString sql = QString("select sum from brand where factory = '%1' and name ='%2'").arg(factoryStr)
            .arg(brandStr);//封包
    query.exec(sql);//执行sql语句
    int sum = 0;
    while(query.next())
    {
        sum = query.value(0).toInt();
    }
    int sell = sum - lastLab;


   sql = QString("update  brand set sell = \
                          '%1',last = '%2' \
         where factory = '%3' and name ='%4'").arg(sell).arg(lastLab).arg(factoryStr)
            .arg(brandStr);//封包
    query.exec(sql);//执行sql语句

   //初始设置失能，确认后设置使能并提示请选择厂家
   ui->ButtonSure->setEnabled(false);
   on_ButtonCancle_clicked();
}
