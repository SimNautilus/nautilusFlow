
#--------------------------------------------------------------

# Global timestep output options
timeStepToStartOutputAt=0
forceOutputAtFirstCall=False

# Global screenshot output options
imageFileNamePadding=0
rescale_lookuptable=False

# Whether or not to request specific arrays from the adaptor.
requestSpecificArrays=False

# a root directory under which all Catalyst output goes
#rootDirectory='/users/fnewberry/PHASTA_Steering/2D_geom14p51_iges_32k/1-procs_test/catalyst_data'
#rootDirectory='/users/fnewberry/PHASTA_Sensei/runs/2D_geom14p51_59k/12-1-Chef/runs_scalability/base_case/catalyst_data'
#rootDirectory='/projects/VisComm/fnewberr/runs/2D_geom14p51_59k/base_sensei/catalyst_data'
#rootDirectory='/lus/theta-fs0/projects/PHASTA_aesp/fnewberr/blower_sensitivity/24-1-Chef/base_dev/catalyst_data'
rootDirectory='../catalyst_data'

# makes a cinema D index table
make_cinema_table=False

#--------------------------------------------------------------
# Code generated from cpstate.py to create the CoProcessor.
# paraview version 5.7.0-899-g504df0f2ea
#--------------------------------------------------------------

from paraview.simple import *
from paraview import coprocessing

# ----------------------- CoProcessor definition -----------------------

def CreateCoProcessor():
  def _CreatePipeline(coprocessor, datadescription):
    class Pipeline:
      # state file generated using paraview version 5.7.0-899-g504df0f2ea

      # ----------------------------------------------------------------
      # setup the data processing pipelines
      # ----------------------------------------------------------------

      # trace generated using paraview version 5.7.0-899-g504df0f2ea
      #
      # To ensure correct image size when batch processing, please search 
      # for and uncomment the line `# renderView*.ViewSize = [*,*]`

      #### disable automatic camera reset on 'Show'
      paraview.simple._DisableFirstRenderCameraReset()

      # create a new 'Phasta Reader'
      # create a producer from a simulation input
      data = coprocessor.CreateProducer(datadescription, 'data')

      # create a new 'Slice'
      # data = FindSource('data')
      slice1 = Slice(Input=data)
      slice1.SliceType = 'Plane'
      slice1.SliceOffsetValues = [0.0]

      # init the 'Plane' selected for 'SliceType'
      slice1.SliceType.Normal = [0.0, 0.0, 1.0]
      slice1.SliceType.Origin = [0.0, 0.0, -0.1]

      # create a new 'Clip'
      clip1 = Clip(Input=slice1)
      clip1.ClipType = 'Box'
      clip1.ClipType.Position = [0.1, 0, -0.1]
      clip1.ClipType.Rotation = [0.0, 0.0, 7.0]
      clip1.ClipType.Length = [1.0, 0.1, 0.1]
      
      # create a new 'Calculator'
      calculator1 = Calculator(Input=clip1)
      calculator1.ResultArrayName = 'u_x2'
      calculator1.Function = 'ybar_X'

      # create a new 'Clip'
      clip2 = Clip(Input=calculator1)
      clip2.ClipType = 'Scalar'
      clip2.Scalars = ['POINTS', 'u_x2']
      clip2.Value = -0.001

      # create a new 'Integrate Variables'
      integrateVariables1 = IntegrateVariables(Input=clip2)
      
      ### Lower blower
      # create a new 'Clip'
      clip3 = Clip(Input=slice1)
      clip3.ClipType = 'Box'

      clip3.ClipType.Position = [0.1, -0.2, -0.1]
      clip3.ClipType.Rotation = [0.0, 0.0, 7.0]
      clip3.ClipType.Length = [1.2, 0.2, 0.1]

      # create a new 'Calculator'
      calculator2 = Calculator(Input=clip3)
      calculator2.ResultArrayName = 'u_x2'
      calculator2.Function = 'ybar_X'

      # create a new 'Clip'
      clip4 = Clip(Input=calculator2)
      clip4.ClipType = 'Scalar'
      clip4.Scalars = ['POINTS', 'u_x2']
      clip4.Value = -0.001

      # create a new 'Integrate Variables'
      integrateVariables2 = IntegrateVariables(Input=clip4)

      # ----------------------------------------------------------------
      # finally, restore active source
      SetActiveSource(integrateVariables2)
      # ----------------------------------------------------------------

      # Now any catalyst writers - it's possible that the writer has to be created when source is active. 
      #cSVWriter1 = servermanager.writers.CSVWriter(Input=slice1)
      #coprocessor.RegisterWriter(cSVWriter1, filename='Slice1_%t.csv', freq=7, paddingamount=0, DataMode='None', HeaderType='None', EncodeAppendedData=None, CompressorType='None', CompressionLevel='None')
      
      cSVWriter1 = servermanager.writers.CSVWriter(Input=integrateVariables1)
      coprocessor.RegisterWriter(cSVWriter1, filename='upper_%t.csv', freq=10, paddingamount=0, DataMode='None', HeaderType='None', EncodeAppendedData=None, CompressorType='None', CompressionLevel='None')
      
      cSVWriter2 = servermanager.writers.CSVWriter(Input=integrateVariables2)
      coprocessor.RegisterWriter(cSVWriter2, filename='lower_%t.csv', freq=10, paddingamount=0, DataMode='None', HeaderType='None', EncodeAppendedData=None, CompressorType='None', CompressionLevel='None')

    return Pipeline()

  class CoProcessor(coprocessing.CoProcessor):
    def CreatePipeline(self, datadescription):
      self.Pipeline = _CreatePipeline(self, datadescription)

#    ### START MODIFICATION ###
#    def WriteData(self, datadescription):
#        #overridden to save extra data out
#        super(CoProcessor, self).WriteData(datadescription)
#
#        # let's write to CSV ourselves.
#        myfilter_upper = self.Pipeline.integrateVariables1
#        fname_upper = 'run_data/upper_%d.csv' % datadescription.GetTimeStep()
#        SaveData(fname_upper, myfilter_upper)
#        myfilter_lower = self.Pipeline.integrateVariables2
#        fname_lower = 'run_data/lower_%d.csv' % datadescription.GetTimeStep()
#        SaveData(fname_lower, myfilter_lower)
#    ### END MODIFICATION ###
  
  coprocessor = CoProcessor()
  # these are the frequencies at which the coprocessor updates.
  # freqs = {'data': [10]}
  # Alteration from earlier version - freq is second entry
  # still does not work... 
  # It should be that the first number is the live output freq - and the second the write frequency. - seems I have to update in the writer itself. 
  freqs = {'data': [5, 10, 15]} 
  coprocessor.SetUpdateFrequencies(freqs)

  if requestSpecificArrays:
    arrays = [['DivQ', 0], ['dwal', 0], ['ev', 0], ['EVbar', 0], ['p', 0], ['pde-res', 0], ['rms-vel', 0], ['u', 0], ['ybar', 0]]
    coprocessor.SetRequestedArrays('data', arrays)
  coprocessor.SetInitialOutputOptions(timeStepToStartOutputAt,forceOutputAtFirstCall)

  if rootDirectory:
      coprocessor.SetRootDirectory(rootDirectory)

  if make_cinema_table:
      coprocessor.EnableCinemaDTable()

  return coprocessor


#--------------------------------------------------------------
# Global variable that will hold the pipeline for each timestep
# Creating the CoProcessor object, doesn't actually create the ParaView pipeline.
# It will be automatically setup when coprocessor.UpdateProducers() is called the
# first time.
coprocessor = CreateCoProcessor()

#--------------------------------------------------------------
# Enable Live-Visualizaton with ParaView and the update frequency
coprocessor.EnableLiveVisualization(True, 6)
#coprocessor.EnableLiveVisualization(False, 6)

# ---------------------- Data Selection method ----------------------

def RequestDataDescription(datadescription):
    "Callback to populate the request for current timestep"
    global coprocessor

    # setup requests for all inputs based on the requirements of the
    # pipeline.
    coprocessor.LoadRequestedData(datadescription)

# ------------------------ Processing method ------------------------

def DoCoProcessing(datadescription):
    "Callback to do co-processing for current timestep"
    global coprocessor

    # Update the coprocessor by providing it the newly generated simulation data.
    # If the pipeline hasn't been setup yet, this will setup the pipeline.
    coprocessor.UpdateProducers(datadescription)

    # Write output data, if appropriate.
    coprocessor.WriteData(datadescription);

    # Write image capture (Last arg: rescale lookup table), if appropriate.
    coprocessor.WriteImages(datadescription, rescale_lookuptable=rescale_lookuptable,
        image_quality=0, padding_amount=imageFileNamePadding)

    # Live Visualization, if enabled.
    #coprocessor.DoLiveVisualization(datadescription, "viz002", 22222)
    coprocessor.DoLiveVisualization(datadescription, "viz003", 22222)
    #coprocessor.DoLiveVisualization(datadescription, "cc013",22222)
