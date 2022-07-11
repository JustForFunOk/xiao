b = ble("Arduino");
c = characteristic(b,"AAAA","19B10001-E8F2-537E-4F6C-D104768A1214");
% callback when receive notify of new data is ready
c.DataAvailableFcn = @displayCharacteristicData;

% TODO: exit when any key is pressed.
% unsubscribe(c);

function displayCharacteristicData(src,evt)
    [data,timestamp] = read(src,'oldest');
    disp(data);
    disp(timestamp);
end
