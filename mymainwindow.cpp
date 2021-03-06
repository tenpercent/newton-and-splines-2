#include "mymainwindow.hh"
#include "plotfield.hh"

MyMainWindow::MyMainWindow()
{
  mainmenu = new QMenuBar();
  quit = new QAction("&Quit", this);

  run = new QAction("&Just do it!", this);
  connect(run, SIGNAL(triggered()), SLOT( push_go() ));

  newton_method = new QAction("&Newton polynomials", this);
  newton_method->setCheckable(true);
  //newton_method->setChecked(true);
  connect(newton_method, SIGNAL(triggered(bool)), SLOT(push_method_1()));

  splines_method = new QAction("&Splines", this);
  splines_method->setCheckable(true);
  connect(splines_method, SIGNAL(triggered(bool)), SLOT(push_method_2()));

  graphic_plot = new QAction("&Plot", this);
  graphic_plot->setCheckable(true);
  graphic_plot->setChecked(true);
  connect(graphic_plot, SIGNAL(triggered()), SLOT(push_graphic()));

  residual_plot = new QAction("&Residual", this);
  residual_plot->setCheckable(true);
  connect(residual_plot, SIGNAL(triggered()), SLOT(push_residual()));

  file = mainmenu->addMenu("&File");
  file->addAction(run);
  file->addSeparator();
  file->addAction(quit);

  method = mainmenu->addMenu("&Choose method...");
  method->addAction(newton_method);
  method->addAction(splines_method);

  plot = mainmenu->addMenu("&Choose plot style...");
  plot->addAction(graphic_plot);
  plot->addAction(residual_plot);

  plot_action_group = new QActionGroup(this);
  graphic_plot->setActionGroup(plot_action_group);
  residual_plot->setActionGroup(plot_action_group);

  connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));

  draw = new PlotField();

  lbl = new QLabel(QString::fromUtf8("Residual: 0.0"));
  count_points=16;
  my_widget = new QWidget();
  my_layout = new QGridLayout(); 
  setMenuBar(mainmenu);
  my_layout->addWidget(draw, 0, 0, 8, 10);
  my_layout->addWidget(points(),10,0,1,1);
  my_layout->addWidget(left(),10,1,1,1);
  my_layout->addWidget(right(),10,2,1,1);
  my_layout->addWidget(lbl,10,3,1,2);
  my_widget->setLayout(my_layout);
  setCentralWidget(my_widget);

  count_points=16;
  start=0;
  finish=1;
  x=NULL; f_x=NULL; /*c1=NULL; c2=NULL, c3=NULL, c4=NULL, */ c=NULL; coef = NULL;
  //draw->method = 1;
  draw->type = 1;

  statusBar()->showMessage("Ready");
}

MyMainWindow::~MyMainWindow(){
  delete[] coef;
  delete[] x;
  delete[] f_x;
  /*
  delete[] c1;
  delete[] c2;
  delete[] c3;
  delete[] c4;
  */
  delete[] c;
  delete draw;
  delete my_layout;
}

QGroupBox *MyMainWindow::points(){
  QGroupBox *group_box = new QGroupBox();
  int maxwidth = 40;
  QLabel *lable = new QLabel;
  QLabel *lable2 = new QLabel;
  lable->setText(QString::fromUtf8("Total points:"));
  lable->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
  lable2->setText(QString::number(count_points));
  lable2->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
  QPushButton *plus=new QPushButton(QString::fromUtf8("+"), this);
  plus -> setMaximumWidth(maxwidth);
  QPushButton *minus=new QPushButton(QChar(0x2212), this);
  minus -> setMaximumWidth(maxwidth);
  connect(plus, SIGNAL(clicked()), SLOT(set_points_plus()));
  connect(minus, SIGNAL(clicked()), SLOT(set_points_minus()));
  connect(this, SIGNAL(counterChanged(int)),lable2, SLOT(setNum(int)));

  QGridLayout *layout = new QGridLayout();
  layout->addWidget(lable, 0, 0, 1, 3);
  layout->addWidget(minus, 1, 0, 1, 1);
  layout->addWidget(lable2, 1, 1, 1, 1);
  layout->addWidget(plus, 1, 2, 1, 1);

  group_box->setAlignment(Qt::AlignHCenter);
  group_box->setLayout(layout); 
  return group_box;
}

QWidget *MyMainWindow::left(){
  QWidget *widget = new QWidget();
  widget->setMaximumHeight(80);
  widget->setMaximumWidth(230);

  QLabel *lable = new QLabel;
  lable->setText(QString::fromUtf8("Left End:"));

  QDoubleSpinBox *spin_box = new QDoubleSpinBox;
  spin_box->setMinimumHeight(32);
  spin_box->setRange(-5000.0, 5000.0);
  spin_box->setSingleStep(0.1);
  spin_box->setValue(0.0);

  //connect(this, SIGNAL(valueChanged(double)), spin_box, SLOT(set_begin(double)));
  connect(spin_box, SIGNAL(valueChanged(double)), SLOT(set_left(double)));

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(lable);
  layout->addWidget(spin_box);
  widget->setLayout(layout);

  return widget;
}

QWidget *MyMainWindow::right(){
  QWidget *widget = new QWidget();
  widget->setMaximumHeight(80);
  widget->setMaximumWidth(230);

  QLabel *lable = new QLabel;
  lable->setText(QString::fromUtf8("Right End:"));

  QDoubleSpinBox *spin_box = new QDoubleSpinBox;
  spin_box->setRange(-5000.0, 5000.0);
  spin_box->setSingleStep(0.1);
  spin_box->setValue(1.0);
  spin_box->setMinimumHeight(32);
  
  //connect(this, SIGNAL(valueChanged(double)), spin_box, SLOT(set_end(double)));
  connect(spin_box, SIGNAL(valueChanged(double)), SLOT(set_right(double)));

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(lable);
  layout->addWidget(spin_box);
  widget->setLayout(layout);

  return widget;
}

void MyMainWindow::set_points_plus(){
  MyMainWindow::count_points =2*count_points;
  if (count_points>=64) 
    if (draw->method==1 || draw->method==3) count_points=64;
  if (count_points>=1000000) count_points=1048576; 
  emit counterChanged(count_points);
}

void MyMainWindow::set_points_minus(){
  MyMainWindow::count_points =count_points/2;
  if (count_points<=2) 
     count_points=2; 
  emit counterChanged(count_points);
}

void MyMainWindow::set_left(double x){
  MyMainWindow::start =x;
}
void MyMainWindow::set_right(double x){
  MyMainWindow::finish =x;
}
void MyMainWindow::push_go()
{
  double r=0.0;
  if(start >finish || (draw->type!= 1 && draw->type!=2) || (draw->method !=1 && draw->method !=2 && draw->method !=3))
  {
    QMessageBox msgBox;
    msgBox.setWindowTitle(QString::fromUtf8("Fatal Error!"));
    msgBox.setText(QString::fromUtf8("JK. Please choose method"));
    msgBox.exec();
    return;
  }
  if (draw->method==1)
  {
    build_newton_coef();
    setup_draw_residual_newton();
    r=residual_newton();
    if (fabs(r)<EPS) 
      r=0.0;
    if (draw->type==1)
    {
      setup_draw_function();
      setup_draw_newton();
      draw->update();
    }
    else
    {
      draw->update();
    }
    lbl->setText(QString::fromUtf8("Residual: ") + QString::number(r));
    return;
  }
  if (draw->method==2)
  {
    build_spline();
    setup_draw_residual_spline();
    r=residual_spline();
    if (fabs(r)<EPS) r=0.0;
    if (draw->type==1)
    {
      setup_draw_function();
      setup_draw_spline();
      draw->update();
    }
    else
      draw->update();
    lbl->setText(QString::fromUtf8("Residual: ") + QString::number(r));  
    draw->update();
  }
  if (draw->method==3)
  {
    lbl->setText(QString::fromUtf8("Please choose one of the methods/n to view the residual"));
    if (draw->type==1)
    {
      build_newton_coef();
      setup_draw_newton();
      build_spline();
      setup_draw_spline();
      draw->update();
    }
    if (draw->type==2)
    {
      build_spline();
      setup_draw_residual_spline();
      build_newton_coef();
      setup_draw_residual_newton();
      draw->update();
    }
  }
  return;
}
void MyMainWindow::push_method_1(){ 
  draw->method ^= 1;
  if (count_points>=64) count_points=64;
  emit counterChanged(count_points);
}

void MyMainWindow::push_method_2(){
  draw->method ^= 2;
}

void MyMainWindow::push_graphic(){ 
  draw->type = 1;
}

void MyMainWindow::push_residual(){
  draw->type = 2;
}

