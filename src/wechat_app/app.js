// app.js
App({
  onLaunch() {
    // 展示本地存储能力
    const logs = wx.getStorageSync('logs') || []
    logs.unshift(Date.now())
    wx.setStorageSync('logs', logs)

    // 登录
    wx.login({
      success: res => {
        // 发送 res.code 到后台换取 openId, sessionKey, unionId
      }
    })
  },
  globalData: {
    userInfo: null,

    // bluetooth和cube页面共享解析后IMU数据
    parsedIMUData: {
      acc_x : 0.0,
      acc_y : 0.0,
      acc_z : 0.0,
    },
  }
})
