import sys
from PyQt4.QtGui import (
    QApplication,
    QGridLayout,
    QMainWindow,
    QTabWidget,
    QWidget,
)

import lFuncs
import lInterfaces
import lTab
import lTabOptions


class lovellGui(QMainWindow):
    def __init__(self, run_data_dir, parent=None):
        QMainWindow.__init__(self, parent)
        self.top_tab = QTabWidget(self)
        self.setCentralWidget(self.top_tab)
        self.top_tab.addTab(velo_view(run_data_dir, self), 'VELO view')
        self.top_tab.addTab(run_view(run_data_dir, self), 'Run view')
        self.top_tab.addTab(QWidget(self), 'Sensor view')
        self.top_tab.addTab(QWidget(self), 'TELL1')
        self.top_tab.addTab(QWidget(self), 'Special analyses')
        self.top_tab.setCurrentIndex(1)
        

class velo_view(QWidget):
    # Class to set up VELO view tab with DQ trending
    def __init__(self, run_data_dir, parent=None):
        QTabWidget.__init__(self, parent)	
        self.run_data_dir = run_data_dir
        self.grid_layout = QGridLayout()
        lFuncs.setPadding(self.grid_layout)
        self.setLayout(self.grid_layout)
        self.pages = [] 
        self.setup_tabs()


    def setup_tabs(self):
        self.top_tab = QTabWidget(self)
        veloview_config = lInterfaces.veloview_config()
	
        self.tab_options = lTabOptions.lTabOptions(self, self.run_data_dir)
        nTabs = len(veloview_config)
        i=1
        for key, val in veloview_config.iteritems():
            msg = 'Preparing tab (' + str(i) + '/' + str(nTabs) + '): ' + val['title']
            print msg
            page = lTab.lTab(val, self)
            if val['title'] == 'Trends': page.modifyPageForTrending(self.run_data_dir,False) 
            if val['title'] == 'Detailed trends': page.modifyPageForTrending(self.run_data_dir,True) 
            self.top_tab.addTab(page, val['title'])
            self.pages.append(page)
            i+=1
        
        self.grid_layout.addWidget(self.top_tab, 0, 0)
        self.top_tab.currentChanged.connect(self.tab_changed)
         
        if self.tab_options:
            self.tab_options.state_change.connect(self.tab_changed)
            self.grid_layout.addWidget(self.tab_options, 1, 0)
            # Keep options stored but don't show corresponding widget in GUI
            self.tab_options.setVisible(False)	
        self.tab_changed()
    
    def tab_changed(self):
        iPage = self.top_tab.currentIndex()
        self.pages[iPage].replotTrend(self.tab_options.state())

class run_view(QWidget):
    def __init__(self, run_data_dir, parent=None):
        QTabWidget.__init__(self, parent)
        print 'run-data-dir set to:', run_data_dir
        self.IV_directory = '/calib/velo/dqm/IVScan'
        #self.IV_directory = '/afs/cern.ch/user/d/dsaunder/IV_test'
        print 'IV_directory set to:', self.IV_directory
        self.run_data_dir = run_data_dir
        self.grid_layout = QGridLayout()
        lFuncs.setPadding(self.grid_layout)
        self.setLayout(self.grid_layout)
        self.pages = [] 
        self.setup_tabs()
        

    def setup_tabs(self):
        self.top_tab = QTabWidget(self)
        runview_config = lInterfaces.runview_config()
        
        # Do the sensor overview adjustments here.
        self.prepSensorOverview(runview_config)
        i=1
        self.tab_options = lTabOptions.lTabOptions(self, self.run_data_dir)
        nTabs = len(runview_config)
        for key, val in runview_config.iteritems():
            msg = 'Preparing tab (' + str(i) + '/' + str(nTabs) + '): ' + val['title']
            print msg
            page = lTab.lTab(val, self)
            if val['title'] == 'IV': page.modifyPageForIV(self.IV_directory)
            self.top_tab.addTab(page, val['title'])
            self.pages.append(page)
            i+=1
            
        self.grid_layout.addWidget(self.top_tab, 0, 1)
        self.top_tab.currentChanged.connect(self.tab_changed)
        
        self.tab_options.state_change.connect(self.tab_changed)
        self.grid_layout.addWidget(self.tab_options, 0, 0)
        msg = "Current run number: " + self.tab_options.state().runNum
        self.tab_options.notify(msg)
        self.tab_changed()

 
    def tab_changed(self):
        iPage = self.top_tab.currentIndex()
        self.pages[iPage].replot(self.tab_options.state(), self.tab_options)
        
        
    def prepSensorOverview(self, config):
        # Find the entry in the dictionary for the sensor overview.
        # Just dictionary gymnastics.
        for key, val in config.iteritems():
            if key == 'sensor_overview':
                plots = []
                # Now loop over all plots in the other tabs (inc. subtabs).
                for key2, val2 in config.iteritems():
                    if key2 == 'sensor_overview': continue      
                    if 'plots' in val2: plots += self.findSensorViewPlots(val2['plots'])
                    elif 'subpages' in val2:
                        for page in val2['subpages']: 
                            if 'plots' in page:
                                plots += self.findSensorViewPlots(page['plots'])


                plotsPerPage=[plots[x:x+4] for x in xrange(0, len(plots), 4)] 
                subpages = []
            
                for i in range(len(plotsPerPage)):
                    subpages.append({'title': str(i), 'plots': plotsPerPage[i]})
                
                if 'subpages' in val: val['subpages'] += subpages
                else: val['subpages'] = subpages
                
            
        
        
    def findSensorViewPlots(self, plot_list):
        plots = []
        for plot in plot_list:
            if 'showInSensorOverview' in plot and plot['showInSensorOverview']:
                plots.append(plot)
              
        return plots


def main(run_data_dir):
    print 'Starting Lovell'
    app = QApplication([]) 
#     form = run_view(run_data_dir)
    form = lovellGui(run_data_dir)
    form.resize(1200, 700)
    form.show() 
    app.setStyle("plastique")
    app.exec_()
