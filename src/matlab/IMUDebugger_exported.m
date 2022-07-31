classdef IMUDebugger_exported < matlab.apps.AppBase

    % Properties that correspond to app components
    properties (Access = public)
        UIFigure             matlab.ui.Figure
        DropDownLabel        matlab.ui.control.Label
        BLEDeviceDropDown    matlab.ui.control.DropDown
        LampLabel            matlab.ui.control.Label
        ConnectStatusLamp    matlab.ui.control.Lamp
        AccelUIAxes          matlab.ui.control.UIAxes
        GyroUIAxes           matlab.ui.control.UIAxes
        SequenceNumberLabel  matlab.ui.control.Label
        XButton              matlab.ui.control.StateButton
        YButton              matlab.ui.control.StateButton
        ZButton              matlab.ui.control.StateButton
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
        
        % plot
        drawX = true;
        drawY = true;
        drawZ = true;
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
        
        function displayCharacteristicData(app, src)
            % Read data
            [binary_data,timestamp] = read(src,'oldest');

            % Deserialize binary msg
            sequence_num = typecast(uint8(binary_data(1:4)), 'uint32');
            accel_x = typecast(uint8(binary_data(5:8)), 'single');
            accel_y = typecast(uint8(binary_data(9:12)), 'single');
            accel_z = typecast(uint8(binary_data(13:16)), 'single');
            gyro_x = typecast(uint8(binary_data(17:20)), 'single');
            gyro_y = typecast(uint8(binary_data(21:24)), 'single');
            gyro_z = typecast(uint8(binary_data(25:28)), 'single');
 
            % Append new data
            app.IMUSequenceNumBuffer(end+1) = sequence_num;
            app.IMUDataBuffer(:, end+1) = [accel_x accel_y accel_z gyro_x gyro_y gyro_z];
 
            % Update sequence number
            app.SequenceNumberLabel.Text = "Sequence Num: " + string(sequence_num);
%             app.RawDataTextArea.Value = "Sequence num: " + string(sequence_num);
%             + string([accel_x accel_y accel_z gyro_x gyro_y gyro_z]);

            % Draw IMU data curve
            % Update the curve every 10 frame(100ms,10Hz). Too high update frequency
            % will cause plot delay.
            if(mod(sequence_num, 10) == 0)
                [~, data_size] = size(app.IMUSequenceNumBuffer);
                if(data_size > app.maxBufferSize)
                    % keep last maxBufferSize msg
                    app.IMUSequenceNumBuffer = app.IMUSequenceNumBuffer(end-app.maxBufferSize+1:end);
                    app.IMUDataBuffer = app.IMUDataBuffer(:, end-app.maxBufferSize+1:end);
                end

                % calculate x axis value every time to suit data loss case
                % use sequence number to calculate relative time
                plotXAxis = (app.IMUSequenceNumBuffer - app.IMUSequenceNumBuffer(end)) * 0.01;

                % plot accel and gyro
                % 
                hold(app.AccelUIAxes, 'off');
                hold(app.GyroUIAxes, 'off');
                if(app.drawX == true)
                    plot(app.AccelUIAxes, plotXAxis, app.IMUDataBuffer(1,:),'-r');
                    hold(app.AccelUIAxes, 'on');
                    
                    plot(app.GyroUIAxes, plotXAxis, app.IMUDataBuffer(4,:),'-r');
                    hold(app.GyroUIAxes, 'on');
                end
                if(app.drawY == true)
                    plot(app.AccelUIAxes, plotXAxis, app.IMUDataBuffer(2,:),'-g');
                    hold(app.AccelUIAxes, 'on');
                    
                    plot(app.GyroUIAxes, plotXAxis, app.IMUDataBuffer(5,:),'-g');
                    hold(app.GyroUIAxes, 'on');
                end
                if(app.drawZ == true)
                    plot(app.AccelUIAxes, plotXAxis, app.IMUDataBuffer(3,:),'-b');
                    hold(app.AccelUIAxes, 'on');
                    
                    plot(app.GyroUIAxes, plotXAxis, app.IMUDataBuffer(6,:),'-b');
                    hold(app.GyroUIAxes, 'on');
                end

                % Add legend will cause plot delay, do not add it
                % legend(app.AccelUIAxes, 'X', 'Y', 'Z');

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

        % Value changed function: XButton
        function XButtonValueChanged(app, event)
            value = app.XButton.Value;
            app.drawX = value;
        end

        % Value changed function: YButton
        function YButtonValueChanged(app, event)
            value = app.YButton.Value;
            app.drawY = value;
        end

        % Value changed function: ZButton
        function ZButtonValueChanged(app, event)
            value = app.ZButton.Value;
            app.drawZ = value;
        end
    end

    % Component initialization
    methods (Access = private)

        % Create UIFigure and components
        function createComponents(app)

            % Create UIFigure and hide until all components are created
            app.UIFigure = uifigure('Visible', 'off');
            app.UIFigure.Position = [100 100 726 525];
            app.UIFigure.Name = 'MATLAB App';
            app.UIFigure.CloseRequestFcn = createCallbackFcn(app, @UIFigureCloseRequest, true);

            % Create DropDownLabel
            app.DropDownLabel = uilabel(app.UIFigure);
            app.DropDownLabel.HorizontalAlignment = 'right';
            app.DropDownLabel.Visible = 'off';
            app.DropDownLabel.Position = [50 452 66 22];
            app.DropDownLabel.Text = 'Drop Down';

            % Create BLEDeviceDropDown
            app.BLEDeviceDropDown = uidropdown(app.UIFigure);
            app.BLEDeviceDropDown.Items = {'---'};
            app.BLEDeviceDropDown.ValueChangedFcn = createCallbackFcn(app, @BLEDeviceDropDownValueChanged, true);
            app.BLEDeviceDropDown.Tooltip = {'Available bluetooth list.'; 'Auto refresh every 5s.'; 'Choos --- will disconnect device.'};
            app.BLEDeviceDropDown.Position = [50 482 136 22];
            app.BLEDeviceDropDown.Value = '---';

            % Create LampLabel
            app.LampLabel = uilabel(app.UIFigure);
            app.LampLabel.HorizontalAlignment = 'right';
            app.LampLabel.Visible = 'off';
            app.LampLabel.Position = [1 452 36 22];
            app.LampLabel.Text = 'Lamp';

            % Create ConnectStatusLamp
            app.ConnectStatusLamp = uilamp(app.UIFigure);
            app.ConnectStatusLamp.Tooltip = {'BLE connection status.'; 'Gray: No connection.'; 'Blue: Connecting.'; 'Green: Connected.'; 'Red: Disconnecting.'};
            app.ConnectStatusLamp.Position = [19 484 18 18];
            app.ConnectStatusLamp.Color = [1 1 1];

            % Create AccelUIAxes
            app.AccelUIAxes = uiaxes(app.UIFigure);
            title(app.AccelUIAxes, 'Accel')
            xlabel(app.AccelUIAxes, 'time(s)')
            ylabel(app.AccelUIAxes, 'acc(g)')
            app.AccelUIAxes.XTick = [-6 -5 -4 -3 -2 -1 0];
            app.AccelUIAxes.XTickLabel = {'-6'; '-5'; '-4'; '-3'; '-2'; '-1'; '0'};
            app.AccelUIAxes.TitleFontWeight = 'bold';
            app.AccelUIAxes.Tag = 'Plot accel realtime data';
            app.AccelUIAxes.Position = [19 245 300 185];

            % Create GyroUIAxes
            app.GyroUIAxes = uiaxes(app.UIFigure);
            title(app.GyroUIAxes, 'Gyro')
            xlabel(app.GyroUIAxes, 'time(s)')
            ylabel(app.GyroUIAxes, 'gyro(deg/s?)')
            app.GyroUIAxes.XTick = [-6 -5 -4 -3 -2 -1 0];
            app.GyroUIAxes.XTickLabel = {'-6'; '-5'; '-4'; '-3'; '-2'; '-1'; '0'};
            app.GyroUIAxes.TitleFontWeight = 'bold';
            app.GyroUIAxes.Position = [19 49 300 185];

            % Create SequenceNumberLabel
            app.SequenceNumberLabel = uilabel(app.UIFigure);
            app.SequenceNumberLabel.Position = [215 482 160 22];
            app.SequenceNumberLabel.Text = 'Sequence Num: ---';

            % Create XButton
            app.XButton = uibutton(app.UIFigure, 'state');
            app.XButton.ValueChangedFcn = createCallbackFcn(app, @XButtonValueChanged, true);
            app.XButton.Text = 'X';
            app.XButton.FontColor = [1 0 0];
            app.XButton.Position = [72 13 22 22];
            app.XButton.Value = true;

            % Create YButton
            app.YButton = uibutton(app.UIFigure, 'state');
            app.YButton.ValueChangedFcn = createCallbackFcn(app, @YButtonValueChanged, true);
            app.YButton.Text = 'Y';
            app.YButton.FontColor = [0 1 0];
            app.YButton.Position = [103 13 22 22];
            app.YButton.Value = true;

            % Create ZButton
            app.ZButton = uibutton(app.UIFigure, 'state');
            app.ZButton.ValueChangedFcn = createCallbackFcn(app, @ZButtonValueChanged, true);
            app.ZButton.Text = 'Z';
            app.ZButton.FontColor = [0 0 1];
            app.ZButton.Position = [133.5 13 21 22];
            app.ZButton.Value = true;

            % Show the figure after all components are created
            app.UIFigure.Visible = 'on';
        end
    end

    % App creation and deletion
    methods (Access = public)

        % Construct app
        function app = IMUDebugger_exported

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