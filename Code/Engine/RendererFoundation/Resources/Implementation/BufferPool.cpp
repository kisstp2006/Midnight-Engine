#include <RendererFoundation/RendererFoundationPCH.h>

#include <RendererFoundation/Resources/BufferPool.h>
#include <RendererFoundation/Device/Device.h>

ezGALBufferPool::~ezGALBufferPool()
{
  Deinitialize();
}

void ezGALBufferPool::Initialize(const ezGALBufferCreationDescription& desc, ezStringView sDebugName)
{
  EZ_ASSERT_DEBUG(m_EventSubscriptionID == 0, "ezGALBufferPool already initialized");
  m_EventSubscriptionID = ezGALDevice::s_Events.AddEventHandler(ezMakeDelegate(&ezGALBufferPool::GALStaticDeviceEventHandler, this));
  m_Desc = desc;
  m_sDebugName = sDebugName;
}

void ezGALBufferPool::Deinitialize()
{
  if (m_EventSubscriptionID != 0)
  {
    ezGALDevice::s_Events.RemoveEventHandler(m_EventSubscriptionID);
    m_EventSubscriptionID = {};
    for (ezGALBufferHandle hBuffer : m_UsedBuffers)
    {
      ezGALDevice::GetDefaultDevice()->DestroyBuffer(hBuffer);
    }
    m_UsedBuffers.Clear();
    m_UsedBuffers.Compact();
    for (ezGALBufferHandle hBuffer : m_FreeBuffers)
    {
      ezGALDevice::GetDefaultDevice()->DestroyBuffer(hBuffer);
    }
    m_FreeBuffers.Clear();
    m_FreeBuffers.Compact();
  }
}

static ezAtomicInteger32 s_Number = 0;

ezGALBufferHandle ezGALBufferPool::GetNewBuffer() const
{
  ezGALBufferHandle buffer = {};
  if (!m_FreeBuffers.IsEmpty())
  {
    buffer = m_FreeBuffers.PeekBack();
    m_FreeBuffers.PopBack();
    m_UsedBuffers.PushBack(buffer);
    return buffer;
  }

  ezGALDevice* pDevice = ezGALDevice::GetDefaultDevice();
  buffer = pDevice->CreateBuffer(m_Desc);
  if (!m_sDebugName.IsEmpty())
  {
    ezUInt32 uiBla = s_Number.Increment();
    ezStringBuilder sTmp;
    sTmp.SetFormat("{}-{}|{}", m_sDebugName, uiBla, m_UsedBuffers.GetCount() + m_FreeBuffers.GetCount());
    pDevice->GetBuffer(buffer)->SetDebugName(sTmp);
  }
  EZ_ASSERT_DEV(!buffer.IsInvalidated(), "Failed to creat pool buffer");
  m_UsedBuffers.PushBack(buffer);
  return buffer;
}

ezGALBufferHandle ezGALBufferPool::GetCurrentBuffer() const
{
  if (m_UsedBuffers.IsEmpty())
    return ezGALBufferHandle();
  //EZ_ASSERT_DEBUG(!m_UsedBuffers.IsEmpty(), "Need to call GetNewBuffer first");
  return m_UsedBuffers.PeekBack();
}

void ezGALBufferPool::GALStaticDeviceEventHandler(const ezGALDeviceEvent& e)
{
  if (e.m_Type == ezGALDeviceEvent::AfterEndFrame)
  {
    ezGALBufferHandle hKeepAcrossFrames = {};
    if (!m_Desc.m_BufferFlags.IsSet(ezGALBufferUsageFlags::Transient) && !m_UsedBuffers.IsEmpty())
    {
      hKeepAcrossFrames = m_UsedBuffers.PeekBack();
      m_UsedBuffers.PopBack();
    }
    while (!m_UsedBuffers.IsEmpty())
    {
      m_FreeBuffers.PushBack(m_UsedBuffers.PeekBack());
      m_UsedBuffers.PopBack();
    }
    if (!hKeepAcrossFrames.IsInvalidated())
    {
      m_UsedBuffers.PushBack(hKeepAcrossFrames);
    }
  }
}
