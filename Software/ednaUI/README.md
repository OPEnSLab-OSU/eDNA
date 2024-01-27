# ednaUI 🚀

<div><img src="docs/readme_screenshot.png" width="400px"></div>




### Contributors
- [Kawin Pechetratanapanit](https://github.com/kawinie)
- [Nathan Jesudason](https://github.com/NathanJesudason)

### Top Level Index
- [1 About](#1-about)
- [2 Components](#2-components)
- [3 React Redux Store](#3-react-redux-store)


## 1 About

ednaUI is a browser-based application intended to be used as the primary way for controlling the [ednaServer](https://github.com/OPEnSLab-OSU/eDNA-Server) backend. Uses React, Redux Toolkit, Tailwind CSS, and Webpack.

- Real-time status monitoring of sensor data and states
- Task configuration and management
- Utility functions

## 2 Components
- [2.1 Monitoring](#21-monitoring)
    - [2.1.1 SensorTile](#211-sensortile)
    - [2.1.2 StateTable](#212-statetable)
    - [2.1.3 ValveStatus](#213-valvestatus)
- [2.2 NotFound](#22-notfound)
- [2.3 TaskConfig](#23-taskconfig)
    - [2.3.1 ConfigCard](#231-configcard)
    - [2.3.2 ConfigListItem](#232-configlistitem)
    - [2.3.3 SubmitCard](#233-submitcard)
    - [2.3.4 TaskForm](#234-taskform)
- [2.4 Tasks](#24-tasks)
   - [2.4.1 NewTaskInput](#241-newtaskinput)
   - [2.4.2 TaskTile](#242-tasktile)
- [2.5 Test](#25-test)
- [2.6 Utilities](#26-utilities)

### 2.1 Monitoring
These components are associated with the monitoring tab, giving you information about the current state of the sampler.

#### 2.1.1 SensorTile
The component that holds sensor readings.
#### 2.1.2 StateTable
The component that holds current state information
#### 2.1.3 ValveStatus
The component that keeps track of which valve is available, about to be used, being used, or already used.

### 2.2 NotFound
This is the 404 Page

### 2.3 TaskConfig
These components are associated with the task configuration page, which appear when editting a task.

#### 2.3.1 ConfigCard
This component is used by the TaskForm inside TaskConfig.
#### 2.3.2 ConfigListItem
This component contains a button that takes the user to the top of a page.
#### 2.3.3 SubmitCard
This component handles saving, deleting, scheduling, and unscheduling tasks.
#### 2.3.4 TaskForm
This component holds the values for the various parameters of a task.

### 2.4 Tasks
These components are associated with the tasks tab

#### 2.4.1 NewTaskInput
This component handles entering in a new task name
#### 2.4.2 TaskTile
This component visualizes each of the tasks on the task tab

### 2.5 Test
This is a tab only available in development mode that allows the user to test out a few features.
### 2.6 Utilities
This tab provides all of the utility functions, such as hyperflush and rtc update.

## 3 React Redux Store
- [3.1 Actions](#31-actions)
    - [3.1.1 getStatusUpdate](#311-getstatusupdate)
    - [3.1.2 getTaskCollection](#312-gettaskcollection)
    - [3.1.3 createTask](#313-createtask)
    - [3.1.4 getTask](#314-gettask)
    - [3.1.5 updateTask](#315-updatetask)
    - [3.1.6 deleteTask](#316-deletetask)
    - [3.1.7 scheduleTask](#317-scheduletask)
    - [3.1.8 unscheduleTask](#318-unscheduletask)
    - [3.1.9 setLoadingScreen](#319-setloadingscreen)
- [3.2 models](#32-models)
    - [3.2.1 TaskServerSchema](#321-taskserverschema)
    - [3.2.2 StatusServerSchema](#322-statusserverschema)
- [3.3 reducers](#33-reducers)
    - [3.3.1 status](#331-status)
    - [3.3.2 initialTaskCollection](#332-initialtaskcollection)
    - [3.3.3 taskCollection](#333-taskcollection)
    - [3.3.4 loadingScreen](#334-loadingscreen)

### 3.1 Actions
An action describes what to do with new data after an event.

#### 3.1.1 getStatusUpdate
gets information from the sampler such as valves, currentState, utc, pressure...
#### 3.1.2 getTaskCollection
Gets all tasks from the sampler
#### 3.1.3 createTask
Requests to the sampler to make a new task with a provided name
#### 3.1.4 getTask
Gets a particular task based on its ID
#### 3.1.5 updateTask
Sends updated task information to server
#### 3.1.6 deleteTask
Deletes a task based on its ID
#### 3.1.7 scheduleTask
Schedules a saved task, sending its ID to the sampler
#### 3.1.8 unscheduleTask
Unschedules a task based on its ID
#### 3.1.9 setLoadingScreen
Stores whether the loading screen should be shown or not.

### 3.2 Models
Defines objects to store data in

#### 3.2.1 TaskServerSchema
Defines a task object, and a dictionary of tasks is used to store all tasks
#### 3.2.2 StatusServerSchema
Defines a status object.

### 3.3 Reducers
Reducers take new information and update the state

#### 3.3.1 status
Updates status
#### 3.3.2 initialTaskCollection
Used only in develop, creates an initial list of tasks of random IDs
#### 3.3.3 taskCollection
Updates taskCollection
#### 3.3.4 loadingScreen
Updates whether the loadingScreen should be displayed or not.