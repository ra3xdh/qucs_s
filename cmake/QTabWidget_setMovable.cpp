/*
 * Check whether Qt provides QTabWidget::setMovable ().
 */

#include <QTabWidget>
class tab_widget : public QTabWidget
{
public:
  tab_widget (QWidget *parent = 0) : QTabWidget (parent) { this->setMovable (true); }
  ~tab_widget () {}
};

int main()
{
  tab_widget tw;
}
