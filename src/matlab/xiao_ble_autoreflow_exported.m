classdef xiao_ble_autoreflow_exported < matlab.apps.AppBase

    % Properties that correspond to app components
    properties (Access = public)
        UIFigure              matlab.ui.Figure
        GridLayout            matlab.ui.container.GridLayout
        LeftPanel             matlab.ui.container.Panel
        LampLabel             matlab.ui.control.Label
        ConnectStatusLamp     matlab.ui.control.Lamp
        RawDataTextAreaLabel  matlab.ui.control.Label
        RawDataTextArea       matlab.ui.control.TextArea
        RawDataLabel          matlab.ui.control.Label
        DropDownLabel         matlab.ui.control.Label
        BLEDeviceDropDown     matlab.ui.control.DropDown
        CenterPanel           matlab.ui.container.Panel
        AccelUIAxes           matlab.ui.control.UIAxes
        GyroUIAxes            matlab.ui.control.UIAxes
        ParsedPlotLabel       matlab.ui.control.Label
        RightPanel            matlab.ui.container.Panel
        FusedResultLabel      matlab.ui.control.Label
    end

    % Properties that correspond to apps with auto-reflow
    properties (Access = private)
        onePanelWidth = 576;
        twoPanelWidth = 768;
    end


    properties (Access = private)
        % BLE
        ScanBLEDeviceTimer = 0;
        % BLE connect status
        isBLEConnectted = false;
        BLEList = table;
        BLEName = table;
        BLEDevice = 0;
        BLECharacteristic = 0;
        
        % IMU
        % IMUDataFreqHz(100) * IMUDataHistoryS(5)
        maxBufferSize = 500;
        % keep last 5s data, 10ms resolution
        % Sequence number  row:1 col:500 uint32
        IMUSequenceNumBuffer = [];
        % IMU Data Buffer  row:6 col:500 float
        % | accelX  |
        % | accelY  |
        % | accelZ  |
        % | gyroX   |
        % | gyroY   |
        % | gyroZ   | 
        IMUDataBuffer = [];
    end

    methods (Access = private)
        function ScanBLEDeviceTimerCallback(app, obj, event)
            % Scan bluetooth devices
            app.BLEList = blelist;
            % Get the name string array from blelist result
            app.BLEName = app.BLEList.Name;
            % Remove empty device name
            is_BLE_name_empty = cellfun(@isempty, app.BLEName);
            app.BLEName = app.BLEName(~is_BLE_name_empty);
            % Append '---' device to device list, If user choose '---', it
            % will disconnect BLE device
            app.BLEName{end+1} = '---';
            app.BLEName = sort(app.BLEName);
            % Refresh bluetooth device list in drop down items
            app.BLEDeviceDropDown.Items = app.BLEName;
        end

        function displayCharacteristicData(app,src)
            % read data
            [binary_data,timestamp] = read(src,'oldest');

            % deserialize binary msg
            sequence_num = typecast(uint8(binary_data(1:4)), 'uint32');
            accel_x = typecast(uint8(binary_data(5:8)), 'single');
            accel_y = typecast(uint8(binary_data(9:12)), 'single');
            accel_z = typecast(uint8(binary_data(13:16)), 'single');
            gyro_x = typecast(uint8(binary_data(17:20)), 'single');
            gyro_y = typecast(uint8(binary_data(21:24)), 'single');
            gyro_z = typecast(uint8(binary_data(25:28)), 'single');
 
            % append new data
            app.IMUSequenceNumBuffer(end+1) = sequence_num;
            app.IMUDataBuffer(:, end+1) = [accel_x accel_y accel_z gyro_x gyro_y gyro_z];
 
            app.RawDataTextArea.Value = "Sequence num: " + string(sequence_num);
%             + string([accel_x accel_y accel_z gyro_x gyro_y gyro_z]);

            % draw IMU data plot, every 50ms
            if(mod(sequence_num, 5) == 0)
                [~, data_size] = size(app.IMUSequenceNumBuffer);
                if(data_size > app.maxBufferSize)
                    % keep last maxBufferSize msg
                    app.IMUSequenceNumBuffer = app.IMUSequenceNumBuffer(end-app.maxBufferSize+1:end);
                    app.IMUDataBuffer = app.IMUDataBuffer(:, end-app.maxBufferSize+1:end);
                end

                % calculate x axis value every time to suit data loss case
                % use sequence number to calculate relative time
                plotXAxis = (app.IMUSequenceNumBuffer - app.IMUSequenceNumBuffer(end)) * 0.01;

                % plot accel
                plot(app.AccelUIAxes, plotXAxis, app.IMUDataBuffer(1,:),'-r');
                hold(app.AccelUIAxes, 'on');
                plot(app.AccelUIAxes, plotXAxis, app.IMUDataBuffer(2,:),'-g');
                hold(app.AccelUIAxes, 'on');
                plot(app.AccelUIAxes, plotXAxis, app.IMUDataBuffer(3,:),'-b');
                hold(app.AccelUIAxes, 'off');

                % plot gyro
                plot(app.GyroUIAxes, plotXAxis, app.IMUDataBuffer(4,:),'-r');
                hold(app.GyroUIAxes, 'on');
                plot(app.GyroUIAxes, plotXAxis, app.IMUDataBuffer(5,:),'-g');
                hold(app.GyroUIAxes, 'on');
                plot(app.GyroUIAxes, plotXAxis, app.IMUDataBuffer(6,:),'-b');
                hold(app.GyroUIAxes, 'off');
            end
            
            % display content in matlab terminal
            % disp(data);
            % disp(timestamp);
        end
    end

    % Callbacks that handle component events
    methods (Access = private)

        % Code that executes after component creation
        function startupFcn(app)
            % Set BLE initial connection status
            app.isBLEConnectted = false;
            app.ConnectStatusLamp.Color = [0.5 0.5 0.5];

            % Config timer
            % Refernce: https://ww2.mathworks.cn/matlabcentral/answers/346703-how-to-use-timer-callback-in-app-designer
            % Refresh BLE device list every 5 seconds
            app.ScanBLEDeviceTimer = timer('Period',5,'ExecutionMode','fixedSpacing',...
                           'TasksToExecute', Inf);
            app.ScanBLEDeviceTimer.TimerFcn = @app.ScanBLEDeviceTimerCallback;

            % Start timer
            start(app.ScanBLEDeviceTimer); % stop(app.ScanBLEDeviceTimer);
        end

        % Changes arrangement of the app based on UIFigure width
        function updateAppLayout(app, event)
            currentFigureWidth = app.UIFigure.Position(3);
            if(currentFigureWidth <= app.onePanelWidth)
                % Change to a 3x1 grid
                app.GridLayout.RowHeight = {543, 543, 543};
                app.GridLayout.ColumnWidth = {'1x'};
                app.CenterPanel.Layout.Row = 1;
                app.CenterPanel.Layout.Column = 1;
                app.LeftPanel.Layout.Row = 2;
                app.LeftPanel.Layout.Column = 1;
                app.RightPanel.Layout.Row = 3;
                app.RightPanel.Layout.Column = 1;
            elseif (currentFigureWidth > app.onePanelWidth && currentFigureWidth <= app.twoPanelWidth)
                % Change to a 2x2 grid
                app.GridLayout.RowHeight = {543, 543};
                app.GridLayout.ColumnWidth = {'1x', '1x'};
                app.CenterPanel.Layout.Row = 1;
                app.CenterPanel.Layout.Column = [1,2];
                app.LeftPanel.Layout.Row = 2;
                app.LeftPanel.Layout.Column = 1;
                app.RightPanel.Layout.Row = 2;
                app.RightPanel.Layout.Column = 2;
            else
                % Change to a 1x3 grid
                app.GridLayout.RowHeight = {'1x'};
                app.GridLayout.ColumnWidth = {233, '1x', 290};
                app.LeftPanel.Layout.Row = 1;
                app.LeftPanel.Layout.Column = 1;
                app.CenterPanel.Layout.Row = 1;
                app.CenterPanel.Layout.Column = 2;
                app.RightPanel.Layout.Row = 1;
                app.RightPanel.Layout.Column = 3;
            end
        end

        % Value changed function: BLEDeviceDropDown
        function BLEDeviceDropDownValueChanged(app, event)
            choosedDevice = app.BLEDeviceDropDown.Value;

            if(choosedDevice == "Arduino" && app.isBLEConnectted == false)
                stop(app.ScanBLEDeviceTimer);

                app.ConnectStatusLamp.Color = [0 0 1];

                app.BLEDevice = ble(choosedDevice);
                app.BLECharacteristic = characteristic(app.BLEDevice,"AAAA","19B10001-E8F2-537E-4F6C-D104768A1214");
                % callback when receive notify of new data is ready
                % https://ww2.mathworks.cn/matlabcentral/answers/723378-saving-data-of-command-window-from-callback-function
                app.BLECharacteristic.DataAvailableFcn = @(obj,event)app.displayCharacteristicData(obj);

                app.isBLEConnectted = true;
                app.ConnectStatusLamp.Color = [0 1 0];
            elseif(choosedDevice == "---" && app.isBLEConnectted == true)
                app.ConnectStatusLamp.Color = [1 0 0];

                % Disconnect BLE device
                % ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ ble ÿÿÿÿÿÿÿÿÿ
%                 unsubscribe(app.BLECharacteristic);

                app.BLECharacteristic = 0;
                app.BLEDevice = 0;
                app.isBLEConnectted = false;
                app.ConnectStatusLamp.Color = [0.5 0.5 0.5];

                start(app.ScanBLEDeviceTimer);
            end
        end

        % Close request function: UIFigure
        function UIFigureCloseRequest(app, event)
            % Stop timer before close app
            stop(app.ScanBLEDeviceTimer);

            delete(app)
        end
    end

    % Component initialization
    methods (Access = private)

        % Create UIFigure and components
        function createComponents(app)

            % Create UIFigure and hide until all components are created
            app.UIFigure = uifigure('Visible', 'off');
            app.UIFigure.AutoResizeChildren = 'off';
            app.UIFigure.Position = [100 100 872 543];
            app.UIFigure.Name = 'MATLAB App';
            app.UIFigure.CloseRequestFcn = createCallbackFcn(app, @UIFigureCloseRequest, true);
            app.UIFigure.SizeChangedFcn = createCallbackFcn(app, @updateAppLayout, true);

            % Create GridLayout
            app.GridLayout = uigridlayout(app.UIFigure);
            app.GridLayout.ColumnWidth = {233, '1x', 290};
            app.GridLayout.RowHeight = {'1x'};
            app.GridLayout.ColumnSpacing = 0;
            app.GridLayout.RowSpacing = 0;
            app.GridLayout.Padding = [0 0 0 0];
            app.GridLayout.Scrollable = 'on';

            % Create LeftPanel
            app.LeftPanel = uipanel(app.GridLayout);
            app.LeftPanel.Layout.Row = 1;
            app.LeftPanel.Layout.Column = 1;

            % Create LampLabel
            app.LampLabel = uilabel(app.LeftPanel);
            app.LampLabel.HorizontalAlignment = 'right';
            app.LampLabel.Visible = 'off';
            app.LampLabel.Position = [162 445 36 22];
            app.LampLabel.Text = 'Lamp';

            % Create ConnectStatusLamp
            app.ConnectStatusLamp = uilamp(app.LeftPanel);
            app.ConnectStatusLamp.Tooltip = {'BLE connection status.'; 'Gray: No connection.'; 'Blue: Connecting.'; 'Green: Connected.'; 'Red: Disconnecting.'};
            app.ConnectStatusLamp.Position = [180 477 18 18];
            app.ConnectStatusLamp.Color = [1 1 1];

            % Create RawDataTextAreaLabel
            app.RawDataTextAreaLabel = uilabel(app.LeftPanel);
            app.RawDataTextAreaLabel.HorizontalAlignment = 'right';
            app.RawDataTextAreaLabel.Position = [10 377 58 22];
            app.RawDataTextAreaLabel.Text = 'Raw Data';

            % Create RawDataTextArea
            app.RawDataTextArea = uitextarea(app.LeftPanel);
            app.RawDataTextArea.Position = [13 48 208 330];

            % Create RawDataLabel
            app.RawDataLabel = uilabel(app.LeftPanel);
            app.RawDataLabel.FontSize = 20;
            app.RawDataLabel.FontWeight = 'bold';
            app.RawDataLabel.Position = [65 508 128 27];
            app.RawDataLabel.Text = 'Raw Data >>';

            % Create DropDownLabel
            app.DropDownLabel = uilabel(app.LeftPanel);
            app.DropDownLabel.HorizontalAlignment = 'right';
            app.DropDownLabel.Visible = 'off';
            app.DropDownLabel.Position = [27 445 66 22];
            app.DropDownLabel.Text = 'Drop Down';

            % Create BLEDeviceDropDown
            app.BLEDeviceDropDown = uidropdown(app.LeftPanel);
            app.BLEDeviceDropDown.Items = {'---'};
            app.BLEDeviceDropDown.ValueChangedFcn = createCallbackFcn(app, @BLEDeviceDropDownValueChanged, true);
            app.BLEDeviceDropDown.Tooltip = {'Available bluetooth list.'; 'Auto refresh every 5s.'; 'Choos --- will disconnect device.'};
            app.BLEDeviceDropDown.Position = [27 475 136 22];
            app.BLEDeviceDropDown.Value = '---';

            % Create CenterPanel
            app.CenterPanel = uipanel(app.GridLayout);
            app.CenterPanel.Layout.Row = 1;
            app.CenterPanel.Layout.Column = 2;

            % Create AccelUIAxes
            app.AccelUIAxes = uiaxes(app.CenterPanel);
            title(app.AccelUIAxes, 'Accel')
            xlabel(app.AccelUIAxes, 'time(s)')
            ylabel(app.AccelUIAxes, 'acc(g)')
            app.AccelUIAxes.XTick = [-6 -5 -4 -3 -2 -1 0];
            app.AccelUIAxes.XTickLabel = {'-6'; '-5'; '-4'; '-3'; '-2'; '-1'; '0'};
            app.AccelUIAxes.TitleFontWeight = 'bold';
            app.AccelUIAxes.Tag = 'Plot accel realtime data';
            app.AccelUIAxes.Position = [14 253 300 185];

            % Create GyroUIAxes
            app.GyroUIAxes = uiaxes(app.CenterPanel);
            title(app.GyroUIAxes, 'Gyro')
            xlabel(app.GyroUIAxes, 'time(s)')
            ylabel(app.GyroUIAxes, 'gyro(deg/s?)')
            app.GyroUIAxes.XTick = [-6 -5 -4 -3 -2 -1 0];
            app.GyroUIAxes.XTickLabel = {'-6'; '-5'; '-4'; '-3'; '-2'; '-1'; '0'};
            app.GyroUIAxes.TitleFontWeight = 'bold';
            app.GyroUIAxes.Position = [14 48 300 185];

            % Create ParsedPlotLabel
            app.ParsedPlotLabel = uilabel(app.CenterPanel);
            app.ParsedPlotLabel.FontSize = 20;
            app.ParsedPlotLabel.FontWeight = 'bold';
            app.ParsedPlotLabel.Position = [126 508 145 27];
            app.ParsedPlotLabel.Text = 'Parsed Plot >>';

            % Create RightPanel
            app.RightPanel = uipanel(app.GridLayout);
            app.RightPanel.Layout.Row = 1;
            app.RightPanel.Layout.Column = 3;

            % Create FusedResultLabel
            app.FusedResultLabel = uilabel(app.RightPanel);
            app.FusedResultLabel.FontSize = 20;
            app.FusedResultLabel.FontWeight = 'bold';
            app.FusedResultLabel.Position = [66 508 130 27];
            app.FusedResultLabel.Text = 'Fused Result';

            % Show the figure after all components are created
            app.UIFigure.Visible = 'on';
        end
    end

    % App creation and deletion
    methods (Access = public)

        % Construct app
        function app = xiao_ble_autoreflow_exported

            % Create UIFigure and components
            createComponents(app)

            % Register the app with App Designer
            registerApp(app, app.UIFigure)

            % Execute the startup function
            runStartupFcn(app, @startupFcn)

            if nargout == 0
                clear app
            end
        end

        % Code that executes before app deletion
        function delete(app)

            % Delete UIFigure when app is deleted
            delete(app.UIFigure)
        end
    end
end